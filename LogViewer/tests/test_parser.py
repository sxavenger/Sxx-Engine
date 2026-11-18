import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))

from sxxlog.parser import decode_bytes, group_entries, parse_bytes, parse_text  # noqa: E402

SAMPLE_DIR = ROOT.parent / "SampleLog"
F1 = SAMPLE_DIR / "2026-11-18_12-55-20.log"
F2 = SAMPLE_DIR / "2026-11-18_13-00-29.log"


@unittest.skipUnless(F1.exists() and F2.exists(), "SampleLog が無い")
class SampleLogTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.r1 = parse_bytes(F1.read_bytes())
        cls.r2 = parse_bytes(F2.read_bytes())

    def test_header(self):
        self.assertEqual(self.r1["header"], {
            "profile": "Debug", "main thread id": "31388", "timestamp": "2026-11-18 12:55:20"})
        self.assertEqual(self.r2["header"]["main thread id"], "32380")

    def test_counts_and_facets_file1(self):
        r = self.r1
        self.assertEqual(r["counts"], {"logs": 1022, "exceptions": 0, "raw": 0})
        self.assertEqual(r["facets"]["label"], {"Stack": 608, "Stream": 414})
        # grep -c で独立に数えた値
        self.assertEqual(r["facets"]["level"], {"Debug": 823, "Info": 199})
        self.assertEqual(sum(r["facets"]["thread"].values()), 1022)
        self.assertEqual(r["facets"]["thread"]["main thread"], 884)
        # メッセージ中の "[file:line] [x]" に引きずられていないこと
        self.assertNotIn("NGXLoadConfig:1145", r["facets"]["thread"])

    def test_counts_and_facets_file2(self):
        r = self.r2
        self.assertEqual(r["counts"], {"logs": 651, "exceptions": 1, "raw": 0})
        # Stream/Stack は従来どおり。Exception は Label facet に "Exception" として加わる
        self.assertEqual(r["facets"]["label"], {"Stream": 397, "Stack": 254, "Exception": 1})
        self.assertEqual(r["facets"]["level"], {"Debug": 468, "Info": 183})
        self.assertEqual(r["facets"]["thread"]["main thread"], 525 + 1)  # Exception も Thread facet に数える
        self.assertEqual(sum(r["facets"]["thread"].values()), 651 + 1)

    def test_message_with_embedded_brackets(self):
        e = next(e for e in self.r1["entries"] if "NGXLoadConfig:1145] [dlss]" in e.get("message", ""))
        self.assertEqual(e["file"], "Engine/Runtime/Graphics/NGXContext.cpp")
        self.assertEqual(e["line"], 29)
        self.assertEqual(e["level"], "Debug")

    def test_first_entry(self):
        e = self.r1["entries"][0]
        self.assertEqual(e["kind"], "log")
        self.assertEqual(e["label"], "Stream")
        self.assertEqual(e["file"], "Engine/Unit/EngineUnit.cpp")
        self.assertEqual(e["line"], 91)
        self.assertEqual(e["thread"], "main thread")
        self.assertEqual(e["level"], "Info")
        self.assertEqual(e["lineno"], 6)
        self.assertEqual(e["message"], "Sxavenger Engine >> version: 4.0.0-Development")

    def test_exception(self):
        excs = [e for e in self.r2["entries"] if e["kind"] == "exception"]
        self.assertEqual(len(excs), 1)
        x = excs[0]
        self.assertEqual(x["title"], "Sxavenger Stream Logger Exception.")
        self.assertEqual(x["file"], "Game/Unit/SandboxUnit.cpp")
        self.assertEqual(x["function"], "void __cdecl SandboxUnit::InitSandbox(void)")
        self.assertEqual(x["line"], 145)
        self.assertEqual(x["thread"], "main thread")
        self.assertEqual(x["ts"], "2026-11-18 13:00:36")
        self.assertEqual(len(x["frames"]), 8)
        self.assertEqual(x["more_frames"], 7)
        self.assertEqual(x["message"], "test exception")
        self.assertEqual(x["lineno"], 657)
        f0, f2 = x["frames"][0], x["frames"][2]
        self.assertEqual((f0["module"], f0["symbol"], f0["offset"], f0["file"], f0["line"]),
                         ("Sandbox", "SandboxUnit::InitSandbox", "0x87F", "SandboxUnit.cpp", 145))
        self.assertEqual(f2["line"], 1792)  # "1,792"
        self.assertEqual(x["frames"][3]["line"], 1005)

    def test_callsite_grouping(self):
        flat = self.r1["entries"]
        g = group_entries(flat, "callsite")
        counts = [e["count"] for e in g if e["kind"] == "stack_group"]
        self.assertEqual(max(counts), 120)
        self.assertEqual(sum(counts) + sum(1 for e in g if e["kind"] != "stack_group"), len(flat))
        grp = next(e for e in g if e.get("count") == 120)
        self.assertEqual(len(grp["children"]), 120)
        self.assertEqual(grp["first_ts"], grp["children"][0]["ts"])
        self.assertEqual(grp["last_ts"], grp["children"][-1]["ts"])
        # 元データは破壊されない
        self.assertEqual(len(self.r1["entries"]), 1022)

    def test_exact_grouping_has_none(self):
        g = group_entries(self.r1["entries"], "exact")
        self.assertFalse([e for e in g if e["kind"] == "stack_group"])
        self.assertEqual(len(g), len(self.r1["entries"]))

    def test_none_grouping(self):
        self.assertEqual(group_entries(self.r1["entries"], "none"), self.r1["entries"])

    def test_invalid_mode(self):
        with self.assertRaises(ValueError):
            group_entries([], "bogus")


LINE = "<Stream> [2026-01-01 00:00:00] [{f}:{l}] [{t}] [{lv}] {m}"


class SyntheticTests(unittest.TestCase):
    def test_windows_path(self):
        t = LINE.format(f="C:/Users/x/a.cpp", l=12, t="main thread", lv="Warning", m="hello")
        e = parse_text(t)["entries"][0]
        self.assertEqual((e["file"], e["line"], e["level"]), ("C:/Users/x/a.cpp", 12, "Warning"))

    def test_multiline_message_and_blank(self):
        t = LINE.format(f="a.cpp", l=1, t="thread id: 1952", lv="Error", m="first") + "\r\n\r\nsecond\r\nthird\r\n"
        r = parse_text(t)
        self.assertEqual(len(r["entries"]), 1)
        self.assertEqual(r["entries"][0]["message"], "first\nsecond\nthird")

    def test_raw_entry_without_previous(self):
        r = parse_text("garbage line\nmore\n" + LINE.format(f="a", l=1, t="t", lv="Info", m="x"))
        self.assertEqual([e["kind"] for e in r["entries"]], ["raw", "raw", "log"])
        self.assertEqual(r["entries"][0]["lineno"], 1)

    def test_no_header_and_lf(self):
        r = parse_text(LINE.format(f="a", l=1, t="t", lv="Info", m="x") + "\n")
        self.assertEqual(r["header"], {})
        self.assertEqual(len(r["entries"]), 1)

    def test_header_unknown_key(self):
        t = "Sxavenger Library File Logger\nprofile: Release\nfoo: bar baz\n" + "#" * 40 + "\n"
        t += LINE.format(f="a", l=1, t="t", lv="Info", m="x")
        r = parse_text(t)
        self.assertEqual(r["header"], {"profile": "Release", "foo": "bar baz"})
        self.assertEqual(len(r["entries"]), 1)
        self.assertEqual(r["entries"][0]["lineno"], 5)

    def test_empty_message(self):
        e = parse_text("<Stream> [t] [a.cpp:1] [main thread] [Info] ")["entries"][0]
        self.assertEqual(e["message"], "")

    def test_exception_missing_terminator(self):
        t = "=" * 40 + "\n\n [Exception] boom\n\n[location]\n filepath: A/b.cpp\n function: f()\n line:     7\n\n" \
            "[thread]\n main thread\n\n[timestamp]\n 2026-01-01 00:00:00\n\n[stacktrace]\n" \
            " 0# M!f+0x1 | b.cpp:7\n 1# garbled frame\n ... (more 3 frames)\n\nmsg line1\nmsg line2\n"
        r = parse_text(t)
        x = r["entries"][0]
        self.assertEqual(len(r["entries"]), 1)
        self.assertEqual((x["title"], x["file"], x["line"]), ("boom", "A/b.cpp", 7))
        self.assertEqual(x["more_frames"], 3)
        self.assertEqual(len(x["frames"]), 2)
        self.assertEqual(x["frames"][1]["raw"], "1# garbled frame")
        self.assertIsNone(x["frames"][1]["file"])
        self.assertEqual(x["message"], "msg line1\nmsg line2")

    def test_raw_lines_in_label_facet(self):
        r = parse_text("junk\n" + LINE.format(f="a", l=1, t="t", lv="Info", m="x"))
        self.assertEqual(r["facets"]["label"], {"Stream": 1, "(raw)": 1})
        self.assertEqual(r["counts"]["raw"], 1)

    def test_separator_without_exception_is_continuation(self):
        sep = "=" * 30
        log = LINE.format(f="a", l=1, t="t", lv="Info", m="first")
        r = parse_text(f"{log}\n{sep}\n\nnot an exception\n{sep}\n")
        self.assertEqual([e["kind"] for e in r["entries"]], ["log"])
        self.assertEqual(r["entries"][0]["message"], f"first\n{sep}\nnot an exception\n{sep}")
        # 先頭にあれば raw 扱い
        r = parse_text(f"{sep}\nhello\n{sep}\n")
        self.assertEqual([e["kind"] for e in r["entries"]], ["raw"] * 3)
        # 区切りだけで EOF
        self.assertEqual([e["kind"] for e in parse_text(sep + "\n")["entries"]], ["raw"])
        # 空行を挟んでも [Exception] が先頭なら Exception
        r = parse_text(f"{log}\n{sep}\n\n\n [Exception] x\n{sep}\n")
        self.assertEqual([e["kind"] for e in r["entries"]], ["log", "exception"])

    def test_log_after_exception_not_appended(self):
        sep = "=" * 30
        t = f"{sep}\n [Exception] e\n{sep}\n" + LINE.format(f="a", l=1, t="t", lv="Info", m="x")
        r = parse_text(t)
        self.assertEqual([e["kind"] for e in r["entries"]], ["exception", "log"])

    def test_decoding(self):
        self.assertEqual(decode_bytes("日本語".encode("utf-8")), "日本語")
        self.assertEqual(decode_bytes("日本語".encode("cp932")), "日本語")
        self.assertIn("\ufffd", decode_bytes(b"\xff\xfe\x80\x81\x00\xe3"))
        self.assertEqual(decode_bytes(b"\xef\xbb\xbfabc"), "abc")

    def test_grouping_boundaries(self):
        def s(i, line=1, msg="m", thread="t"):
            return LINE.replace("<Stream>", "<Stack>").format(f="a.cpp", l=line, t=thread, lv="Debug", m=msg)
        text = "\n".join([s(0), s(1), s(2, line=2), s(3, msg="other"), s(4, msg="other"),
                          LINE.format(f="a.cpp", l=1, t="t", lv="Debug", m="m"), s(6)])
        ents = parse_text(text)["entries"]
        cs = group_entries(ents, "callsite")
        self.assertEqual([e.get("count", 1) for e in cs], [2, 1, 2, 1, 1])
        ex = group_entries(ents, "exact")
        self.assertEqual([e.get("count", 1) for e in ex], [2, 1, 2, 1, 1])
        # message だけが違う連続: callsite ではまとまり、exact ではまとまらない
        ab = parse_text(s(0, msg="a") + "\n" + s(1, msg="b"))["entries"]
        self.assertEqual([e.get("count", 1) for e in group_entries(ab, "callsite")], [2])
        self.assertEqual([e.get("count", 1) for e in group_entries(ab, "exact")], [1, 1])
        # callsite: 3,4 は line=1 で 0,1 と同じ key だが 2 を挟むので別グループ
        self.assertEqual(cs[0]["count"], 2)
        self.assertEqual(cs[2]["count"], 2)
        self.assertEqual(cs[2]["message"], "other")


if __name__ == "__main__":
    unittest.main()
