"""Sxavenger Library File Logger のログパーサ (UI 非依存)。

parse_bytes() / parse_text() でフラットなエントリ列を作り、
group_entries() で同じ Label の連続行をまとめる。
"""
from __future__ import annotations

import re
from collections import Counter
from typing import Any

HEADER_TITLE = "Sxavenger Library File Logger"

_LOG_RE = re.compile(
    r"^<(?P<label>[^>]+)> \[(?P<ts>[^\]]+)\] \[(?P<file>.+?):(?P<line>\d+)\] "
    r"\[(?P<thread>[^\]]+)\] \[(?P<level>[^\]]+)\] ?(?P<msg>.*)$"
)
_SEP_RE = re.compile(r"^={20,}\s*$")
_HASH_RE = re.compile(r"^#{10,}\s*$")
_KV_RE = re.compile(r"^\s*([^:]+?)\s*:\s*(.*?)\s*$")
_EXC_TITLE_RE = re.compile(r"^\s*\[Exception\]\s*(.*?)\s*$")
_SECTION_RE = re.compile(r"^\s*\[(location|thread|timestamp|stacktrace)\]\s*$")
_FRAME_RE = re.compile(
    r"^\s*(?P<idx>\d+)#\s+(?:(?P<mod>[^!\s]+)!)?(?P<sym>.*?)"
    r"(?:\+(?P<off>0x[0-9A-Fa-f]+))?\s*\|\s*(?P<file>.+?):(?P<line>[\d,]+)\s*$"
)
_MORE_RE = re.compile(r"^\s*\.\.\.\s*\(more\s+(\d+)\s+frames?\)\s*$")
_FRAME_START_RE = re.compile(r"^\s*\d+#")
_SPLIT_RE = re.compile(r"\r\n|\n|\r")

GROUP_MODES = ("callsite", "exact", "none")


def decode_bytes(data: bytes) -> str:
    """utf-8 -> cp932 -> utf-8(replace) の順でデコード。BOM は除去。"""
    for enc in ("utf-8", "cp932"):
        try:
            text = data.decode(enc)
            break
        except UnicodeDecodeError:
            continue
    else:
        text = data.decode("utf-8", errors="replace")
    return text[1:] if text.startswith("﻿") else text


def _starts_exception(lines: list[str], i: int) -> bool:
    """区切り行の次の非空行が [Exception] を含むときだけ Exception ブロックとみなす。"""
    n = len(lines)
    while i < n and not lines[i].strip():
        i += 1
    return i < n and "[Exception]" in lines[i]


def _parse_exception(lines: list[str], lineno: int) -> dict[str, Any]:
    ent: dict[str, Any] = {
        "kind": "exception", "title": "", "file": None, "function": None,
        "line": None, "thread": None, "ts": None, "frames": [],
        "more_frames": 0, "message": "", "lineno": lineno,
    }
    section: str | None = None
    msg: list[str] = []
    want_value = False  # [thread]/[timestamp] の次の非空行待ち
    for raw in lines:
        if not raw.strip():
            continue
        m = _EXC_TITLE_RE.match(raw)
        if m and section is None:
            ent["title"] = m.group(1)
            continue
        m = _SECTION_RE.match(raw)
        if m:
            section = m.group(1)
            want_value = section in ("thread", "timestamp")
            continue
        if section == "location":
            kv = _KV_RE.match(raw)
            if kv:
                key, val = kv.group(1).lower(), kv.group(2)
                if key == "filepath":
                    ent["file"] = val
                elif key == "function":
                    ent["function"] = val
                elif key == "line":
                    try:
                        ent["line"] = int(val.replace(",", ""))
                    except ValueError:
                        ent["line"] = None
        elif section in ("thread", "timestamp"):
            if want_value:
                ent["thread" if section == "thread" else "ts"] = raw.strip()
                want_value = False
            else:
                msg.append(raw.strip())
        elif section == "stacktrace":
            mm = _MORE_RE.match(raw)
            if mm:
                ent["more_frames"] = int(mm.group(1))
                continue
            fm = _FRAME_RE.match(raw)
            if fm:
                ent["frames"].append({
                    "index": int(fm.group("idx")),
                    "module": fm.group("mod"),
                    "symbol": fm.group("sym"),
                    "offset": fm.group("off"),
                    "file": fm.group("file"),
                    "line": int(fm.group("line").replace(",", "")),
                    "raw": raw.strip(),
                })
            elif _FRAME_START_RE.match(raw):
                idx = int(raw.strip().split("#", 1)[0])
                ent["frames"].append({
                    "index": idx, "module": None, "symbol": None, "offset": None,
                    "file": None, "line": None, "raw": raw.strip(),
                })
            else:
                msg.append(raw.strip())
        else:
            msg.append(raw.strip())
    ent["message"] = "\n".join(msg)
    return ent


def _parse_header(lines: list[str]) -> tuple[dict[str, str], int]:
    """ヘッダを解析し (header, 次に読む行 index) を返す。無ければ ({}, 0)。"""
    i = 0
    n = len(lines)
    while i < n and not lines[i].strip():
        i += 1
    if i >= n or lines[i].strip() != HEADER_TITLE:
        return {}, 0
    header: dict[str, str] = {}
    i += 1
    while i < n:
        line = lines[i]
        if _HASH_RE.match(line):
            return header, i + 1
        if _LOG_RE.match(line) or _SEP_RE.match(line):
            return header, i  # 区切り行が無い場合
        kv = _KV_RE.match(line)
        if kv:
            header[kv.group(1)] = kv.group(2)
        i += 1
    return header, i


def parse_text(text: str) -> dict[str, Any]:
    lines = _SPLIT_RE.split(text)
    header, i = _parse_header(lines)
    entries: list[dict[str, Any]] = []
    n = len(lines)
    last_log: dict[str, Any] | None = None

    while i < n:
        line = lines[i]
        lineno = i + 1
        if _SEP_RE.match(line) and _starts_exception(lines, i + 1):
            j = i + 1
            block: list[str] = []
            while j < n and not _SEP_RE.match(lines[j]):
                block.append(lines[j])
                j += 1
            entries.append(_parse_exception(block, lineno))
            last_log = None
            i = j + 1  # 終端区切り欠落(EOF)でも n+1 になるだけで問題なし
            continue
        if not line.strip():
            i += 1
            continue
        m = _LOG_RE.match(line) if line.startswith("<") else None
        if m:
            last_log = {
                "kind": "log", "label": m.group("label"), "ts": m.group("ts"),
                "file": m.group("file"), "line": int(m.group("line")),
                "thread": m.group("thread"), "level": m.group("level"),
                "message": m.group("msg"), "lineno": lineno,
            }
            entries.append(last_log)
        elif last_log is not None:
            last_log["message"] += "\n" + line
        else:
            entries.append({"kind": "raw", "text": line, "lineno": lineno})
        i += 1

    for idx, e in enumerate(entries):
        e["id"] = idx

    facets: dict[str, Counter] = {"label": Counter(), "thread": Counter(), "level": Counter()}
    n_log = n_exc = n_raw = 0
    for e in entries:
        if e["kind"] == "log":
            n_log += 1
            facets["label"][e["label"]] += 1
            facets["thread"][e["thread"]] += 1
            facets["level"][e["level"]] += 1
        elif e["kind"] == "exception":
            n_exc += 1
            # フィルター UI で 0 件にできるよう、Exception / raw も Label facet に含める
            facets["label"]["Exception"] += 1
            if e["thread"]:
                facets["thread"][e["thread"]] += 1
        else:
            n_raw += 1
            facets["label"]["(raw)"] += 1
    return {
        "header": header,
        "entries": entries,
        "facets": {k: dict(v.most_common()) for k, v in facets.items()},
        "counts": {"logs": n_log, "exceptions": n_exc, "raw": n_raw},
    }


def parse_bytes(data: bytes) -> dict[str, Any]:
    return parse_text(decode_bytes(data))


def _group_key(e: dict[str, Any], mode: str):
    base = (e["label"], e["file"], e["line"], e["thread"], e["level"])
    return base + (e["message"],) if mode == "exact" else base


def group_entries(entries: list[dict[str, Any]], mode: str = "callsite") -> list[dict[str, Any]]:
    """連続する通常ログ(Label が同じもの)を mode に従ってまとめた新しいリストを返す(純関数)。"""
    if mode not in GROUP_MODES:
        raise ValueError(f"unknown group mode: {mode}")
    if mode == "none":
        return list(entries)
    out: list[dict[str, Any]] = []
    run: list[dict[str, Any]] = []

    def flush() -> None:
        if not run:
            return
        if len(run) == 1:
            out.append(run[0])
        else:
            first = run[0]
            out.append({
                "id": first["id"], "kind": "group", "label": first["label"],
                "first_ts": first["ts"], "last_ts": run[-1]["ts"],
                "file": first["file"], "line": first["line"],
                "thread": first["thread"], "level": first["level"],
                "message": first["message"], "count": len(run),
                "lineno": first["lineno"], "children": list(run),
            })
        run.clear()

    for e in entries:
        if e["kind"] == "log":
            if run and _group_key(run[0], mode) != _group_key(e, mode):
                flush()
            run.append(e)
        else:
            flush()
            out.append(e)
    flush()
    return out


def parse_and_group(data: bytes, mode: str = "callsite") -> dict[str, Any]:
    result = parse_bytes(data)
    result["entries"] = group_entries(result["entries"], mode)
    result["group_mode"] = mode
    return result
