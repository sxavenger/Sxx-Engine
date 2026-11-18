"""ネイティブのフォルダ選択ダイアログ (サーバが動いている PC 上で開く)。

Tk / PowerShell はいずれも **別プロセス** で実行し、結果を UTF-8 の stdout で受け取る
(サーバのスレッド内で GUI を触らない)。タイムアウトは設けない (ユーザー操作待ち)。
"""
from __future__ import annotations

import json
import os
import subprocess
import sys

DEFAULT_TITLE = "フォルダを選択"


class PickerUnavailable(Exception):
    """どの方法でもダイアログを開けない。"""


# 子プロセス (tkinter)。結果は {"path": str|null} か {"error": "no_tk"} を 1 行の JSON で出力する。
TK_SCRIPT = r'''
import json, sys
try:
    sys.stdout.reconfigure(encoding="utf-8")
except Exception:
    pass
try:
    import tkinter
    from tkinter import filedialog
    root = tkinter.Tk()
except Exception:
    print(json.dumps({"error": "no_tk"}))
    sys.exit(0)
root.withdraw()
try:
    root.attributes("-topmost", True)
except Exception:
    pass
try:
    root.update()
    path = filedialog.askdirectory(initialdir=sys.argv[1] or None, title=sys.argv[2], parent=root, mustexist=True)
finally:
    root.destroy()
print(json.dumps({"path": path or None}))
'''

# PowerShell フォールバック。title / initial は環境変数で渡す (クォート問題の回避)。
PS_SCRIPT = r'''
$ErrorActionPreference = 'Stop'
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
Add-Type -AssemblyName System.Windows.Forms
$owner = New-Object System.Windows.Forms.Form
$owner.TopMost = $true
$d = New-Object System.Windows.Forms.FolderBrowserDialog
$d.Description = $env:SXX_PICK_TITLE
$d.ShowNewFolderButton = $true
if ($env:SXX_PICK_INITIAL) { $d.SelectedPath = $env:SXX_PICK_INITIAL }
if ($d.ShowDialog($owner) -eq [System.Windows.Forms.DialogResult]::OK) { [Console]::Out.Write($d.SelectedPath) }
'''


def _is_windows() -> bool:
    return os.name == "nt"


def _run(cmd: list[str], env: dict[str, str] | None = None) -> subprocess.CompletedProcess:
    kwargs: dict = dict(capture_output=True, text=True, encoding="utf-8", errors="replace",
                        stdin=subprocess.DEVNULL, env=env)
    if _is_windows():
        kwargs["creationflags"] = getattr(subprocess, "CREATE_NO_WINDOW", 0)  # コンソール窓を出さない
    return subprocess.run(cmd, **kwargs)  # タイムアウトなし


def _norm(path: str | None) -> str | None:
    return os.path.normpath(path) if path else None


def _pick_tk(initial: str, title: str) -> dict | None:
    """{"path": ...} (選択/キャンセル) か None (tkinter が使えない)。"""
    try:
        p = _run([sys.executable, "-c", TK_SCRIPT, initial, title])
    except OSError:
        return None
    for line in reversed((p.stdout or "").splitlines()):
        try:
            obj = json.loads(line)
        except ValueError:
            continue
        if isinstance(obj, dict) and "path" in obj:
            return {"path": _norm(obj["path"])}
        break
    return None  # no_tk / クラッシュ / 出力なし


def _pick_powershell(initial: str, title: str) -> dict | None:
    env = dict(os.environ, SXX_PICK_TITLE=title, SXX_PICK_INITIAL=initial)
    try:
        p = _run(["powershell.exe", "-NoProfile", "-STA", "-NonInteractive", "-ExecutionPolicy", "Bypass",
                  "-Command", PS_SCRIPT], env=env)
    except OSError:
        return None
    if p.returncode != 0:
        return None
    out = (p.stdout or "").lstrip("﻿").strip()
    return {"path": _norm(out)}


def pick_folder(initial: str = "", title: str = DEFAULT_TITLE) -> dict:
    """{"path": str | None} を返す (None = キャンセル)。開けなければ PickerUnavailable。"""
    title = title or DEFAULT_TITLE
    res = _pick_tk(initial, title)
    if res is None and _is_windows():
        res = _pick_powershell(initial, title)
    if res is None:
        raise PickerUnavailable("フォルダ選択ダイアログを開けません。")
    return res
