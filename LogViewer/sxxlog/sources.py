"""ソースファイル解決。常に source_root 配下に限定する (セキュリティ重要)。"""
from __future__ import annotations

import ntpath
import os
import posixpath
import threading
import time
from pathlib import Path, PurePosixPath, PureWindowsPath

from .parser import decode_bytes

MAX_SOURCE_BYTES = 5 * 1024 * 1024
MAX_INDEX_FILES = 500_000

EXCLUDED_DIRS = {
    ".git", ".svn", ".hg", ".vs", ".idea", "node_modules", "__pycache__",
    "build", "bin", "obj", "x64", "x86", "intermediate", "out", "packages",
}
INDEX_STALE_SECONDS = 30

LANGUAGE_BY_EXT = {
    ".cpp": "cpp", ".cc": "cpp", ".cxx": "cpp", ".c": "c", ".h": "cpp", ".hpp": "cpp",
    ".hh": "cpp", ".hxx": "cpp", ".inl": "cpp", ".ixx": "cpp", ".cppm": "cpp",
    ".hlsl": "cpp", ".hlsli": "cpp", ".fx": "cpp", ".fxh": "cpp", ".usf": "cpp",  # Monaco 標準に hlsl が無いので cpp
    ".py": "python", ".toml": "ini", ".ini": "ini", ".cfg": "ini",
    ".json": "json", ".xml": "xml", ".md": "markdown", ".txt": "plaintext",
    ".cs": "csharp", ".js": "javascript", ".ts": "typescript", ".bat": "bat",
    ".cmake": "plaintext", ".yml": "yaml", ".yaml": "yaml", ".html": "html",
    ".css": "css", ".sh": "shell", ".ps1": "powershell", ".lua": "lua",
}


def classify_path(p: str) -> str:
    """ファイルシステムに触れずにパス種別を判定する (Windows / POSIX 両方の解釈で見る)。

    "unc" (//server/share, \\\\server\\share) / "drive_relative" (C:foo) /
    "absolute" (C:/x, /x, \\x) / "relative"
    """
    p = p.replace("\\", "/")
    if p.startswith("//"):
        return "unc"
    w = PureWindowsPath(p)
    if w.drive:
        return "absolute" if w.root else "drive_relative"
    if w.root or PurePosixPath(p).is_absolute():
        return "absolute"
    return "relative"


def is_safe_relative(p: str) -> bool:
    """/vendor/ 用: 絶対・UNC・ドライブ指定・'..' で上位へ出るものを拒否 (FS には触れない)。"""
    if not p or "\0" in p or classify_path(p) != "relative":
        return False
    parts = [x for x in p.replace("\\", "/").split("/") if x not in ("", ".")]
    depth = 0
    for x in parts:
        depth += -1 if x == ".." else 1
        if depth < 0:
            return False
    return True


def _lexical_under(root: Path, p: str, kind: str) -> str | None:
    """root 配下なら正規化済みパス文字列、そうでなければ None (FS には触れない)。"""
    nt = os.name == "nt"
    mod, flavor = (ntpath, PureWindowsPath) if nt else (posixpath, PurePosixPath)
    base = str(root)
    lex = mod.normpath(p if kind == "absolute" else mod.join(base, p))
    return lex if flavor(lex).is_relative_to(flavor(base)) else None


class SourceError(Exception):
    """code: no_root | invalid | outside_root | not_found | not_file | too_large | binary"""

    def __init__(self, code: str, message: str):
        super().__init__(message)
        self.code = code
        self.message = message


def language_for(path: str | Path) -> str:
    return LANGUAGE_BY_EXT.get(Path(str(path)).suffix.lower(), "plaintext")


class SourceResolver:
    def __init__(self, root: str | Path | None):
        self._root_raw = str(root) if root else ""
        self.root: Path | None = None
        if root:
            try:
                p = Path(root).expanduser().resolve()
                if p.is_dir():
                    self.root = p
            except (OSError, RuntimeError):
                self.root = None
        self._index: dict[str, list[str]] | None = None
        self._index_time = 0.0
        self._clock = time.monotonic
        self._lock = threading.Lock()

    # ---- 解決 -------------------------------------------------------------
    def _require_root(self) -> Path:
        if self.root is None:
            raise SourceError("no_root", "source_root が未設定、または存在しません。設定から指定してください。")
        return self.root

    def resolve(self, path: str) -> Path:
        """path を source_root 基準で解決し、配下であることを保証して返す。"""
        root = self._require_root()
        if not isinstance(path, str) or not path.strip() or "\0" in path:
            raise SourceError("invalid", "パスが不正です。")
        p = path.strip().replace("\\", "/")
        # --- ここまでは FS に触れず、文字列だけで判定する ---
        kind = classify_path(p)
        if kind in ("unc", "drive_relative"):
            raise SourceError("outside_root", "UNC / ドライブ相対のパスは開けません。")
        lex = _lexical_under(root, p, kind)
        if lex is None:
            raise SourceError("outside_root", "source_root の外にあるパスは開けません。")
        try:
            cand = Path(lex).resolve()  # symlink はここで解決し、下で再度配下確認
        except (OSError, RuntimeError, ValueError):
            raise SourceError("invalid", "パスを解決できません。")
        if not cand.is_relative_to(root):
            raise SourceError("outside_root", "source_root の外にあるパスは開けません。")
        if not cand.exists():
            raise SourceError("not_found", f"ファイルが見つかりません: {p}")
        if not cand.is_file():
            raise SourceError("not_file", f"ファイルではありません: {p}")
        return cand

    def rel(self, abs_path: Path) -> str:
        return PurePosixPath(abs_path.relative_to(self._require_root())).as_posix()

    def read(self, path: str) -> dict:
        abs_path = self.resolve(path)
        try:
            size = abs_path.stat().st_size
            if size > MAX_SOURCE_BYTES:
                raise SourceError("too_large", f"ファイルが大きすぎます ({size} bytes > {MAX_SOURCE_BYTES})。")
            data = abs_path.read_bytes()
        except OSError as e:
            raise SourceError("not_found", f"読み込めません: {e.strerror or e}")
        if b"\0" in data[:8192]:
            raise SourceError("binary", "バイナリファイルは表示できません。")
        return {
            "path": self.rel(abs_path),
            "abs": str(abs_path),
            "language": language_for(abs_path),
            "content": decode_bytes(data),
        }

    # ---- ファイル名検索 ---------------------------------------------------
    def _build_index(self) -> dict[str, list[str]]:
        root = self._require_root()
        index: dict[str, list[str]] = {}
        count = 0
        for dirpath, dirnames, filenames in os.walk(root, followlinks=False):
            dirnames[:] = [d for d in dirnames if d.lower() not in EXCLUDED_DIRS]
            for fn in filenames:
                rel = PurePosixPath(Path(dirpath, fn).relative_to(root)).as_posix()
                index.setdefault(fn.lower(), []).append(rel)
                count += 1
            if count >= MAX_INDEX_FILES:
                break
        for v in index.values():
            v.sort()
        return index

    def find_by_name(self, name: str) -> list[str]:
        """basename で source_root 以下を検索し、相対パス(/区切り)の一覧を返す。"""
        self._require_root()
        if not isinstance(name, str) or not name.strip():
            return []
        name = name.strip()
        if "/" in name or "\\" in name or "\0" in name:
            return []
        out = self._lookup(name, rebuild=False)
        if not out:
            with self._lock:
                stale = self._index is not None and self._clock() - self._index_time > INDEX_STALE_SECONDS
            if stale:  # 古いインデックスで 0 件なら作り直して再検索
                out = self._lookup(name, rebuild=True)
        return out

    def _lookup(self, name: str, rebuild: bool) -> list[str]:
        with self._lock:
            if self._index is None or rebuild:
                self._index = self._build_index()
                self._index_time = self._clock()
            index = self._index
        out = []
        for rel in index.get(name.lower(), []):
            try:
                self.resolve(rel)  # symlink で外に出る候補は除外
            except SourceError:
                continue
            out.append(rel)
        return out

    def reset_index(self) -> None:
        with self._lock:
            self._index = None
