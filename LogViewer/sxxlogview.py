#!/usr/bin/env python3
"""SxxLogViewer エントリポイント。

使い方: python sxxlogview.py [LOGFILE] [--source-root DIR] [--log-dir DIR] [--port N] [--no-browser]
"""
from __future__ import annotations

import argparse
import sys
import threading
import webbrowser
from pathlib import Path
from urllib.parse import quote

APP_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(APP_DIR))

from sxxlog.server import ConfigStore, make_server  # noqa: E402


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="Sxavenger Engine ログビューア")
    ap.add_argument("logfile", nargs="?", help="起動時に開くログファイル (親ディレクトリを log_dir として保存します)")
    ap.add_argument("--source-root", help="エンジンのリポジトリルート (ログ中の相対パスの基準)")
    ap.add_argument("--log-dir", help="ログのあるディレクトリ")
    ap.add_argument("--port", type=int, default=0, help="待ち受けポート (既定: 空きポート)")
    ap.add_argument("--keep-alive", action="store_true",
                    help="ブラウザのタブを閉じても終了しない (既定: タブが全部閉じたら自動終了。Ctrl+C でも終了可)")
    ap.add_argument("--exit-grace", type=float, default=5.0, metavar="秒",
                    help="全タブが閉じてから終了するまでの猶予秒 (既定: 5。再読み込みで落ちないための余裕)")
    ap.add_argument("--no-browser", action="store_true", help="ブラウザを自動で開かない")
    args = ap.parse_args(argv)
    for stream in (sys.stdout, sys.stderr):  # Windows コンソール(cp932)でも落ちないように
        try:
            stream.reconfigure(errors="replace")
        except (AttributeError, ValueError):
            pass

    cfg = ConfigStore(APP_DIR / "config.json", default_log_dir=APP_DIR.parent / "SampleLog")

    changes: dict[str, str] = {}
    initial_log: str | None = None
    if args.logfile:
        lf = Path(args.logfile).expanduser()
        if not lf.is_file():
            print(f"エラー: ログファイルが見つかりません: {lf}", file=sys.stderr)
            return 2
        lf = lf.resolve()
        initial_log = lf.name
        changes["log_dir"] = str(lf.parent)
    if args.log_dir:
        changes["log_dir"] = args.log_dir
    if args.source_root:
        changes["source_root"] = args.source_root
    if changes:
        try:
            cfg.update(changes)
        except ValueError as e:
            print(f"エラー: {e}", file=sys.stderr)
            return 2

    try:
        server = make_server(APP_DIR, cfg, args.port, auto_exit=not args.keep_alive, exit_grace=max(0.0, args.exit_grace))
    except OSError as e:
        if args.port:
            print(f"エラー: ポート {args.port} を使用できません ({e.strerror or e})。"
                  "別のポートを --port で指定するか、省略して空きポートを自動選択してください。", file=sys.stderr)
        else:
            print(f"エラー: サーバを起動できません ({e.strerror or e})。", file=sys.stderr)
        return 1
    url = f"http://127.0.0.1:{server.port}/"
    if initial_log:
        url += "?log=" + quote(initial_log)
    print(f"SxxLogViewer: {url}  (Ctrl+C で終了" + ("" if args.keep_alive else " / ブラウザのタブを全部閉じても終了") + ")")
    snap = cfg.snapshot()
    print(f"  source_root: {snap['source_root'] or '(未設定)'}")
    print(f"  log_dir    : {snap['log_dir'] or '(未設定)'}")
    if not args.no_browser:
        threading.Timer(0.3, lambda: webbrowser.open(url)).start()
    try:
        server.serve_forever()
        if server.exit_reason == "viewer_closed":
            print("ビューアが閉じられたため終了します。")
    except KeyboardInterrupt:
        print("\n終了します。")
    finally:
        server.server_close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
