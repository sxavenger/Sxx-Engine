"""http.server ベースのローカルサーバと JSON API (127.0.0.1 のみ)。"""
from __future__ import annotations

import gzip
import json
import mimetypes
import os
import select
import sys
import threading
import time
from http import HTTPStatus
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from typing import Any
from urllib.parse import parse_qs, urlsplit, unquote

from . import picker as picker_mod
from .parser import GROUP_MODES, group_entries, parse_bytes
from .sources import SourceError, SourceResolver, is_safe_relative

MAX_UPLOAD_BYTES = 100 * 1024 * 1024
THEMES = ("light", "dark")
MONACO_VERSION = "0.52.2"
CDN = "https://cdn.jsdelivr.net/npm"

# アセット名 -> (vendor 配下の相対パス, CDN URL)
ASSETS = {
    "DAISYUI_CSS": ("daisyui.css", f"{CDN}/daisyui@5/daisyui.css"),
    "THEMES_CSS": ("themes.css", f"{CDN}/daisyui@5/themes.css"),
    "TAILWIND_JS": ("tailwindcss-browser.js", f"{CDN}/@tailwindcss/browser@4"),
    "MONACO_VS": ("monaco/min/vs/loader.js", f"{CDN}/monaco-editor@{MONACO_VERSION}/min/vs"),
}

MIME_OVERRIDES = {
    ".js": "text/javascript; charset=utf-8", ".mjs": "text/javascript; charset=utf-8",
    ".css": "text/css; charset=utf-8", ".json": "application/json; charset=utf-8",
    ".ttf": "font/ttf", ".woff": "font/woff", ".woff2": "font/woff2",
    ".map": "application/json; charset=utf-8", ".svg": "image/svg+xml",
}

ERROR_STATUS = {
    "no_root": HTTPStatus.NOT_FOUND, "not_found": HTTPStatus.NOT_FOUND,
    "outside_root": HTTPStatus.FORBIDDEN, "invalid": HTTPStatus.BAD_REQUEST,
    "not_file": HTTPStatus.BAD_REQUEST, "too_large": HTTPStatus.REQUEST_ENTITY_TOO_LARGE,
    "binary": HTTPStatus.UNSUPPORTED_MEDIA_TYPE,
}


class ConfigStore:
    DEFAULTS = {"source_root": "", "log_dir": "", "group_mode": "callsite", "theme": "dark"}

    def __init__(self, path: Path, default_log_dir: Path | None = None):
        self.path = path
        self._lock = threading.Lock()
        self.data = dict(self.DEFAULTS)
        loaded = False
        if path.is_file():
            try:
                raw = json.loads(path.read_text(encoding="utf-8-sig"))  # BOM 付きも可
                if isinstance(raw, dict):
                    for k in self.DEFAULTS:
                        if isinstance(raw.get(k), str):
                            self.data[k] = raw[k]
                    loaded = True
            except (OSError, ValueError):
                pass
        if not self.data["log_dir"] and default_log_dir and default_log_dir.is_dir():
            self.data["log_dir"] = str(default_log_dir)
        if self.data["group_mode"] not in GROUP_MODES:
            self.data["group_mode"] = "callsite"
        if self.data["theme"] not in THEMES:
            self.data["theme"] = "dark"
        self.loaded = loaded
        self.warning = ""  # 保存に失敗したときの警告 (動作は継続)

    def snapshot(self) -> dict[str, str]:
        with self._lock:
            return dict(self.data)

    def update(self, changes: dict[str, Any]) -> dict[str, str]:
        """検証して更新・保存。不正なら ValueError(メッセージは日本語)。"""
        new: dict[str, str] = {}
        for key, val in changes.items():
            if key not in self.DEFAULTS:
                continue
            if not isinstance(val, str):
                raise ValueError(f"{key} は文字列で指定してください。")
            val = val.strip()
            if key in ("source_root", "log_dir"):
                if val:
                    p = Path(val).expanduser()
                    if not p.is_dir():
                        raise ValueError(f"ディレクトリが存在しません: {val}")
                    val = str(p.resolve())
            elif key == "group_mode" and val not in GROUP_MODES:
                raise ValueError(f"group_mode は {', '.join(GROUP_MODES)} のいずれかです。")
            elif key == "theme" and val not in THEMES:
                raise ValueError(f"theme は {', '.join(THEMES)} のいずれかです。")
            new[key] = val
        with self._lock:
            self.data = dict(self.data, **new)
            try:
                self._save_locked(self.data)
                self.warning = ""
            except OSError as e:  # 書き込めなくても今回の起動中は設定を使い続ける
                self.warning = f"config.json を保存できません (この設定は今回の起動中のみ有効): {e.strerror or e}"
                print("警告: " + self.warning, file=sys.stderr)
            return dict(self.data)

    def _save_locked(self, data: dict[str, str]) -> None:
        tmp = self.path.with_suffix(".json.tmp")
        tmp.write_text(json.dumps(data, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
        tmp.replace(self.path)


class AliveMonitor:
    """SSE(/api/alive) の接続数を数え、接続が一度でもあった後に 0 の状態が grace 秒続いたら on_expire を呼ぶ。"""

    def __init__(self, grace: float, on_expire):
        self.grace = grace
        self.on_expire = on_expire
        self._lock = threading.Lock()
        self.count = 0
        self.ever_connected = False
        self._zero_since = 0.0

    def connect(self) -> None:
        with self._lock:
            self.count += 1
            self.ever_connected = True

    def disconnect(self) -> None:
        with self._lock:
            self.count -= 1
            if self.count <= 0:
                self.count = 0
                self._zero_since = time.monotonic()

    def watch(self, stop: threading.Event) -> None:
        interval = max(0.02, min(0.1, self.grace / 4))
        while not stop.wait(interval):
            with self._lock:
                expired = (self.ever_connected and self.count == 0
                           and time.monotonic() - self._zero_since >= self.grace)
            if expired:
                self.on_expire()
                return


class AppServer(ThreadingHTTPServer):
    daemon_threads = True
    # Windows の SO_REUSEADDR は同一ポートの二重 bind を許してしまうため無効にする
    allow_reuse_address = os.name != "nt"

    def __init__(self, app_dir: Path, config: ConfigStore, host: str = "127.0.0.1", port: int = 0,
                 auto_exit: bool = False, exit_grace: float = 5.0, ping_interval: float = 3.0):
        self.stopping = threading.Event()        # 終了処理中 (SSE スレッドが抜けるための合図)。bind 失敗時の server_close からも参照される
        super().__init__((host, port), Handler)
        self.ping_interval = ping_interval
        self.exit_reason: str | None = None      # "viewer_closed" ならタブが閉じられて自動終了
        self.alive = AliveMonitor(exit_grace, self._expire)
        if auto_exit:
            threading.Thread(target=self.alive.watch, args=(self.stopping,), daemon=True, name="alive-watch").start()
        self.app_dir = app_dir
        self.web_dir = app_dir / "web"
        self.vendor_dir = app_dir / "vendor"
        self.config = config
        self._lock = threading.Lock()
        self._resolver: SourceResolver | None = None
        self._resolver_root: str | None = None
        self._parse_cache: dict[tuple, dict] = {}
        self.picker = picker_mod.pick_folder     # テストで差し替え可能
        self.pick_lock = threading.Lock()        # ダイアログは同時に 1 つだけ

    def _expire(self) -> None:
        self.exit_reason = "viewer_closed"
        self.stopping.set()
        threading.Thread(target=self.shutdown, daemon=True, name="alive-shutdown").start()

    def server_close(self) -> None:
        self.stopping.set()
        super().server_close()

    def handle_error(self, request, client_address):  # 切断などの定常的な例外でトレースを出さない
        if isinstance(sys.exc_info()[1], (BrokenPipeError, ConnectionResetError, ConnectionAbortedError)):
            return
        super().handle_error(request, client_address)

    @property
    def port(self) -> int:
        return self.server_address[1]

    def allowed_hosts(self) -> set[str]:
        return {f"127.0.0.1:{self.port}", f"localhost:{self.port}"}

    def resolver(self) -> SourceResolver:
        root = self.config.snapshot()["source_root"]
        with self._lock:
            if self._resolver is None or self._resolver_root != root:
                self._resolver = SourceResolver(root or None)
                self._resolver_root = root
            return self._resolver

    def log_dir(self) -> Path | None:
        d = self.config.snapshot()["log_dir"]
        if not d:
            return None
        p = Path(d)
        return p if p.is_dir() else None

    def parse_cached(self, path: Path) -> dict:
        st = path.stat()
        key = (str(path), st.st_mtime_ns, st.st_size)
        with self._lock:
            hit = self._parse_cache.get(key)
        if hit is not None:
            return hit
        result = parse_bytes(path.read_bytes())
        with self._lock:
            if len(self._parse_cache) >= 3:
                self._parse_cache.pop(next(iter(self._parse_cache)))
            self._parse_cache[key] = result
        return result

    def asset_urls(self) -> dict[str, str]:
        urls = {}
        for name, (rel, cdn) in ASSETS.items():
            vendor_file = self.vendor_dir / rel
            if vendor_file.is_file():
                url = "/vendor/" + rel
                if name == "MONACO_VS":
                    url = "/vendor/monaco/min/vs"
                urls[name] = url
            else:
                urls[name] = cdn
        return urls

    def render_index(self) -> bytes:
        html = (self.web_dir / "index.html").read_text(encoding="utf-8")
        urls = self.asset_urls()
        for name, url in urls.items():
            html = html.replace("{{ASSET_" + name + "}}", url)
        mode = "vendor" if all(u.startswith("/vendor/") for u in urls.values()) else (
            "cdn" if not any(u.startswith("/vendor/") for u in urls.values()) else "mixed")
        html = html.replace("{{ASSET_MODE}}", mode)
        return html.encode("utf-8")


class Handler(BaseHTTPRequestHandler):
    server: AppServer
    protocol_version = "HTTP/1.1"
    server_version = "SxxLogViewer"

    def log_message(self, fmt, *args):  # 静かに
        pass

    # ---- 共通 -------------------------------------------------------------
    def _send(self, status: int, body: bytes, ctype: str, extra: dict[str, str] | None = None,
              compress: bool = False) -> None:
        if compress and len(body) > 65536 and "gzip" in self.headers.get("Accept-Encoding", ""):
            body = gzip.compress(body, compresslevel=3)
            extra = dict(extra or {}, **{"Content-Encoding": "gzip"})
        self.send_response(status)
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(body)))
        self.send_header("X-Content-Type-Options", "nosniff")
        if self.close_connection:
            self.send_header("Connection", "close")
        for k, v in (extra or {}).items():
            self.send_header(k, v)
        self.end_headers()
        if self.command != "HEAD":
            self.wfile.write(body)

    def _json(self, obj: Any, status: int = 200) -> None:
        body = json.dumps(obj, ensure_ascii=False, separators=(",", ":")).encode("utf-8")
        self._send(status, body, "application/json; charset=utf-8", {"Cache-Control": "no-store"}, compress=True)

    def _error(self, status: int, code: str, message: str) -> None:
        self._json({"error": code, "message": message}, int(status))

    def _check_origin(self) -> bool:
        """DNS rebinding / クロスサイトからの利用を防ぐ。"""
        if self.headers.get("Host", "") not in self.server.allowed_hosts():
            self._error(HTTPStatus.FORBIDDEN, "bad_host", "Host ヘッダが不正です。")
            return False
        origin = self.headers.get("Origin")
        if origin is not None and origin not in {f"http://{h}" for h in self.server.allowed_hosts()}:
            self._error(HTTPStatus.FORBIDDEN, "bad_origin", "Origin が不正です。")
            return False
        return True

    def _read_body(self, limit: int) -> bytes | None:
        raw = self.headers.get("Content-Length")
        if raw is None or not raw.isdigit():
            self._error(HTTPStatus.LENGTH_REQUIRED, "length_required", "Content-Length が必要です。")
            return None
        n = int(raw)
        if n > limit:
            self.close_connection = True
            self._error(HTTPStatus.REQUEST_ENTITY_TOO_LARGE, "too_large", f"本文が大きすぎます (上限 {limit} bytes)。")
            return None
        data = self.rfile.read(n)
        if len(data) != n:
            self.close_connection = True
            return None
        return data

    def _group_param(self, q: dict[str, list[str]]) -> str | None:
        mode = q.get("group", [self.server.config.snapshot()["group_mode"]])[0]
        if mode not in GROUP_MODES:
            self._error(HTTPStatus.BAD_REQUEST, "invalid", f"group は {', '.join(GROUP_MODES)} のいずれかです。")
            return None
        return mode

    # ---- ルーティング -----------------------------------------------------
    def do_HEAD(self):
        self.do_GET()

    def do_GET(self):
        if not self._check_origin():
            return
        url = urlsplit(self.path)
        q = parse_qs(url.query, keep_blank_values=True)
        path = url.path
        try:
            if path in ("/", "/index.html"):
                self._send(200, self.server.render_index(), "text/html; charset=utf-8", {"Cache-Control": "no-store"})
            elif path.startswith("/vendor/"):
                self._static_vendor(unquote(path[len("/vendor/"):]))
            elif path == "/favicon.ico":
                self._send(204, b"", "image/x-icon")
            elif path == "/api/alive":
                self._api_alive()
            elif path == "/api/config":
                self._json(self._config_payload())
            elif path == "/api/logs":
                self._api_logs()
            elif path == "/api/log":
                self._api_log(q)
            elif path == "/api/source":
                self._api_source(q)
            elif path == "/api/find":
                self._api_find(q)
            else:
                self._error(HTTPStatus.NOT_FOUND, "not_found", "見つかりません。")
        except (BrokenPipeError, ConnectionResetError):
            pass
        except Exception as e:  # 想定外でもサーバは落とさない
            self._error(HTTPStatus.INTERNAL_SERVER_ERROR, "internal", f"内部エラー: {e!r}")

    def do_POST(self):
        # 本文を読まずに応答するパスがあるため、keep-alive での本文混入(リクエスト解釈のずれ)を避けて毎回閉じる
        self.close_connection = True
        if not self._check_origin():
            return
        url = urlsplit(self.path)
        q = parse_qs(url.query, keep_blank_values=True)
        try:
            if url.path == "/api/config":
                self._api_post_config()
            elif url.path == "/api/parse":
                self._api_parse(q)
            elif url.path == "/api/pick-folder":
                self._api_pick_folder()
            else:
                self._error(HTTPStatus.NOT_FOUND, "not_found", "見つかりません。")
        except (BrokenPipeError, ConnectionResetError):
            pass
        except Exception as e:
            self._error(HTTPStatus.INTERNAL_SERVER_ERROR, "internal", f"内部エラー: {e!r}")

    # ---- 静的ファイル -----------------------------------------------------
    def _static_vendor(self, rel: str) -> None:
        vendor = self.server.vendor_dir
        # resolve() の前に、絶対 / UNC / ドライブ指定 / '..' 脱出を文字列だけで拒否する
        if not is_safe_relative(rel):
            return self._error(HTTPStatus.NOT_FOUND, "not_found", "見つかりません。")
        try:
            vroot = vendor.resolve()
            target = (vroot / rel.replace("\\", "/")).resolve()
        except (OSError, RuntimeError, ValueError):
            return self._error(HTTPStatus.NOT_FOUND, "not_found", "見つかりません。")
        if not target.is_relative_to(vroot) or not target.is_file():
            return self._error(HTTPStatus.NOT_FOUND, "not_found", "見つかりません。")
        ctype = MIME_OVERRIDES.get(target.suffix.lower()) or mimetypes.guess_type(target.name)[0] or "application/octet-stream"
        self._send(200, target.read_bytes(), ctype, {"Cache-Control": "public, max-age=3600"})

    # ---- API --------------------------------------------------------------
    def _config_payload(self) -> dict:
        cfg = self.server.config.snapshot()
        cfg["assets"] = self.server.asset_urls()
        r = self.server.resolver()
        cfg["source_root_ok"] = r.root is not None
        cfg["warning"] = self.server.config.warning
        return cfg

    def _api_post_config(self) -> None:
        body = self._read_body(1024 * 1024)
        if body is None:
            return
        try:
            obj = json.loads(body.decode("utf-8"))
            if not isinstance(obj, dict):
                raise ValueError("JSON オブジェクトで指定してください。")
            self.server.config.update(obj)
        except (ValueError, UnicodeDecodeError) as e:
            return self._error(HTTPStatus.BAD_REQUEST, "invalid", str(e))
        self.server.resolver().reset_index()
        self._json(self._config_payload())

    def _api_pick_folder(self) -> None:
        body = self._read_body(64 * 1024)
        if body is None:
            return
        try:
            obj = json.loads(body.decode("utf-8") or "{}")
            if not isinstance(obj, dict):
                raise ValueError("JSON オブジェクトで指定してください。")
            initial, title = obj.get("initial") or "", obj.get("title") or picker_mod.DEFAULT_TITLE
            if not isinstance(initial, str) or not isinstance(title, str):
                raise ValueError("initial / title は文字列で指定してください。")
        except (ValueError, UnicodeDecodeError) as e:
            return self._error(HTTPStatus.BAD_REQUEST, "invalid", str(e))
        cfg = self.server.config.snapshot()
        start = ""
        for cand in (initial, cfg["source_root"], cfg["log_dir"]):
            if cand and Path(cand).expanduser().is_dir():
                start = str(Path(cand).expanduser())
                break
        if not self.server.pick_lock.acquire(blocking=False):
            return self._error(HTTPStatus.CONFLICT, "busy", "別のフォルダ選択ダイアログが開いています。")
        try:
            result = self.server.picker(start, title[:200])
        except picker_mod.PickerUnavailable:
            return self._error(HTTPStatus.NOT_IMPLEMENTED, "unavailable",
                               "この環境ではフォルダ選択ダイアログを開けません。テキストで入力してください。")
        finally:
            self.server.pick_lock.release()
        self._json({"path": result.get("path")})

    def _api_alive(self) -> None:
        """生存確認用の SSE。タブが開いている間つなぎっぱなしにし、切断(= タブ/ブラウザを閉じた)をサーバが検知する。"""
        if self.command == "HEAD":
            return self._error(HTTPStatus.METHOD_NOT_ALLOWED, "invalid", "GET のみ対応です。")
        srv = self.server
        self.close_connection = True
        self.send_response(200)
        self.send_header("Content-Type", "text/event-stream; charset=utf-8")
        self.send_header("Cache-Control", "no-cache, no-transform")
        self.send_header("Connection", "close")
        self.send_header("X-Accel-Buffering", "no")
        self.send_header("X-Content-Type-Options", "nosniff")
        self.end_headers()
        try:
            self.wfile.write(b": ok\nretry: 2000\n\n")
            self.wfile.flush()
        except OSError:
            return
        srv.alive.connect()
        try:
            conn, last = self.connection, time.monotonic()
            while not srv.stopping.is_set():
                wait = max(0.0, min(0.25, srv.ping_interval - (time.monotonic() - last)))
                readable, _, _ = select.select([conn], [], [], wait)
                if readable:                      # 読めた = 切断(EOF)か、無視してよい余計なデータ
                    if not conn.recv(4096):
                        break
                    continue
                if time.monotonic() - last >= srv.ping_interval:
                    self.wfile.write(b": ping\n\n")
                    self.wfile.flush()
                    last = time.monotonic()
        except (OSError, ValueError):             # BrokenPipe / ConnectionReset / 閉じたソケット
            pass
        finally:
            srv.alive.disconnect()

    def _api_logs(self) -> None:
        d = self.server.log_dir()
        if d is None:
            return self._json({"log_dir": self.server.config.snapshot()["log_dir"], "logs": []})
        items = []
        for p in d.glob("*.log"):
            try:
                if p.is_file():
                    st = p.stat()
                    items.append({"name": p.name, "size": st.st_size, "mtime": st.st_mtime})
            except OSError:
                continue
        items.sort(key=lambda x: (x["mtime"], x["name"]), reverse=True)
        self._json({"log_dir": str(d), "logs": items})

    def _api_log(self, q) -> None:
        name = q.get("name", [""])[0]
        mode = self._group_param(q)
        if mode is None:
            return
        d = self.server.log_dir()
        if d is None:
            return self._error(HTTPStatus.NOT_FOUND, "no_log_dir", "log_dir が未設定、または存在しません。")
        if (not name or "/" in name or "\\" in name or "\0" in name or name in (".", "..")
                or Path(name).name != name or not name.lower().endswith(".log")):
            return self._error(HTTPStatus.BAD_REQUEST, "invalid", "ログファイル名が不正です (log_dir 直下の *.log のみ)。")
        target = (d / name)
        try:
            real = target.resolve()
            if real.parent != d.resolve() or not real.is_file():
                raise FileNotFoundError
        except (OSError, RuntimeError):
            return self._error(HTTPStatus.NOT_FOUND, "not_found", f"ログが見つかりません: {name}")
        result = self.server.parse_cached(real)
        out = dict(result, entries=group_entries(result["entries"], mode), group_mode=mode, name=name)
        self._json(out)

    def _api_parse(self, q) -> None:
        mode = self._group_param(q)
        if mode is None:
            return
        body = self._read_body(MAX_UPLOAD_BYTES)
        if body is None:
            return
        result = parse_bytes(body)
        result["entries"] = group_entries(result["entries"], mode)
        result["group_mode"] = mode
        self._json(result)

    def _api_source(self, q) -> None:
        path = q.get("path", [""])[0]
        try:
            self._json(self.server.resolver().read(path))
        except SourceError as e:
            self._error(ERROR_STATUS.get(e.code, HTTPStatus.BAD_REQUEST), e.code, e.message)

    def _api_find(self, q) -> None:
        name = q.get("name", [""])[0]
        try:
            self._json({"name": name, "candidates": self.server.resolver().find_by_name(name)})
        except SourceError as e:
            self._error(ERROR_STATUS.get(e.code, HTTPStatus.BAD_REQUEST), e.code, e.message)


def make_server(app_dir: Path, config: ConfigStore, port: int = 0, **kwargs) -> AppServer:
    """kwargs: auto_exit / exit_grace / ping_interval (AppServer 参照)"""
    return AppServer(app_dir, config, "127.0.0.1", port, **kwargs)
