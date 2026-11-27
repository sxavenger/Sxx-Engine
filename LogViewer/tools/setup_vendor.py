#!/usr/bin/env python3
"""オフライン用ライブラリを registry.npmjs.org から取得して Application/vendor/ に配置する。

  python tools/setup_vendor.py [--force] [--dest DIR]

取得物 (標準ライブラリのみ使用 / tarball は npm の integrity(sha512) を検証):
  daisyui              (5.x 最新)  -> vendor/daisyui.css, vendor/themes.css
  @tailwindcss/browser (4.x 最新)  -> vendor/tailwindcss-browser.js
  monaco-editor        (0.52.2)    -> vendor/monaco/min/vs/**   (CDN 版とバージョンを一致させる)
vendor/ があるとサーバは CDN ではなくこちらを配信する。
"""
from __future__ import annotations

import argparse
import base64
import hashlib
import io
import json
import re
import sys
import tarfile
import urllib.request
from pathlib import Path
from urllib.parse import quote

REGISTRY = "https://registry.npmjs.org"
APP_DIR = Path(__file__).resolve().parent.parent
MONACO_VERSION = "0.52.2"  # sxxlog/server.py の MONACO_VERSION と一致させること

# (package, version 指定, [(tar 内パス or ディレクトリ接頭辞, 出力先), ...])
PACKAGES = [
    ("daisyui", "5", [("package/daisyui.css", "daisyui.css"), ("package/themes.css", "themes.css")]),
    ("@tailwindcss/browser", "4", [("package/dist/index.global.js", "tailwindcss-browser.js")]),
    ("monaco-editor", MONACO_VERSION, [("package/min/vs/", "monaco/min/vs/")]),
]

_STABLE = re.compile(r"^(\d+)\.(\d+)\.(\d+)$")


def http_get(url: str, accept: str | None = None) -> bytes:
    req = urllib.request.Request(url, headers={"User-Agent": "sxx-log-viewer-setup"})
    if accept:
        req.add_header("Accept", accept)
    with urllib.request.urlopen(req, timeout=120) as r:
        return r.read()


def pick_version(packument: dict, spec: str) -> str:
    """spec が完全一致 (例 0.52.2) ならそれ、メジャー (例 5) ならその最新安定版。"""
    versions = packument["versions"]
    if spec in versions:
        return spec
    cands = []
    for v in versions:
        m = _STABLE.match(v)
        if m and m.group(1) == spec:
            cands.append((tuple(int(x) for x in m.groups()), v))
    if not cands:
        raise SystemExit(f"{packument.get('name')}: バージョン {spec} が見つかりません")
    return max(cands)[1]


def verify_integrity(data: bytes, dist: dict) -> None:
    integ = dist.get("integrity", "")
    if integ.startswith("sha512-"):
        digest = base64.b64encode(hashlib.sha512(data).digest()).decode()
        if digest != integ[len("sha512-"):]:
            raise SystemExit("integrity (sha512) が一致しません。ダウンロードが壊れているか改ざんされています。")
    elif dist.get("shasum"):
        if hashlib.sha1(data).hexdigest() != dist["shasum"]:
            raise SystemExit("shasum が一致しません。")
    else:
        raise SystemExit("integrity 情報が無いため検証できません。")


def safe_dest(base: Path, rel: str) -> Path:
    dest = (base / rel).resolve()
    if not dest.is_relative_to(base.resolve()):
        raise SystemExit(f"不正なパスを検出しました: {rel}")
    return dest


def extract(data: bytes, wanted: list[tuple[str, str]], dest_dir: Path) -> int:
    n = 0
    with tarfile.open(fileobj=io.BytesIO(data), mode="r:gz") as tar:
        for m in tar:
            if not m.isfile():  # symlink / hardlink / device は無視
                continue
            name = m.name
            if name.startswith("/") or ".." in Path(name).parts:
                raise SystemExit(f"tar に不正なパスがあります: {name}")
            for src, out in wanted:
                if src.endswith("/"):
                    if not name.startswith(src):
                        continue
                    rel = out + name[len(src):]
                elif name == src:
                    rel = out
                else:
                    continue
                dest = safe_dest(dest_dir, rel)
                dest.parent.mkdir(parents=True, exist_ok=True)
                f = tar.extractfile(m)
                if f is None:
                    continue
                dest.write_bytes(f.read())
                n += 1
                break
    return n


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--dest", type=Path, default=APP_DIR / "vendor", help="出力先 (既定: Application/vendor)")
    ap.add_argument("--force", action="store_true", help="既存の vendor を上書きして再取得")
    args = ap.parse_args()
    dest: Path = args.dest
    meta_path = dest / "versions.json"
    if meta_path.is_file() and not args.force:
        print(f"{dest} は取得済みです (--force で再取得)。")
        return 0
    dest.mkdir(parents=True, exist_ok=True)

    installed = {}
    for pkg, spec, wanted in PACKAGES:
        url = f"{REGISTRY}/{quote(pkg, safe='@')}"
        print(f"[{pkg}] メタデータ取得: {url}")
        packument = json.loads(http_get(url, accept="application/vnd.npm.install-v1+json"))
        ver = pick_version(packument, spec)
        dist = packument["versions"][ver]["dist"]
        print(f"[{pkg}] {ver} をダウンロード: {dist['tarball']}")
        data = http_get(dist["tarball"])
        verify_integrity(data, dist)
        n = extract(data, wanted, dest)
        if n == 0:
            raise SystemExit(f"{pkg}: 展開対象のファイルが tarball にありませんでした")
        print(f"[{pkg}] {n} ファイルを展開")
        installed[pkg] = ver

    meta_path.write_text(json.dumps(installed, indent=2) + "\n", encoding="utf-8")
    required = ["daisyui.css", "themes.css", "tailwindcss-browser.js", "monaco/min/vs/loader.js"]
    missing = [r for r in required if not (dest / r).is_file()]
    if missing:
        print("不足: " + ", ".join(missing), file=sys.stderr)
        return 1
    print(f"完了: {dest}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
