import os
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))

from unittest import mock

from sxxlog.sources import (MAX_SOURCE_BYTES, SourceError, SourceResolver, classify_path,  # noqa: E402
                            is_safe_relative, language_for)


class SourceTests(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        base = Path(self._tmp.name).resolve()
        self.root = base / "repo"
        self.outside = base / "outside"
        (self.root / "Engine/Runtime").mkdir(parents=True)
        (self.root / "Game/Unit").mkdir(parents=True)
        (self.root / "Other/Unit").mkdir(parents=True)
        (self.root / "build").mkdir()
        (self.root / "node_modules").mkdir()
        self.outside.mkdir()
        (self.root / "Engine/Runtime/a.cpp").write_text("line1\nline2\n", encoding="utf-8")
        (self.root / "Game/Unit/SandboxUnit.cpp").write_text("x\n" * 10, encoding="utf-8")
        (self.root / "Other/Unit/SandboxUnit.cpp").write_text("y\n", encoding="utf-8")
        (self.root / "build/Hidden.cpp").write_text("h", encoding="utf-8")
        (self.root / "node_modules/Hidden2.cpp").write_text("h", encoding="utf-8")
        (self.outside / "secret.txt").write_text("SECRET", encoding="utf-8")
        self.r = SourceResolver(self.root)

    def tearDown(self):
        self._tmp.cleanup()

    def assertCode(self, code, fn, *a):
        with self.assertRaises(SourceError) as cm:
            fn(*a)
        self.assertEqual(cm.exception.code, code)

    def test_resolve_ok(self):
        p = self.r.resolve("Engine/Runtime/a.cpp")
        self.assertEqual(p, self.root / "Engine/Runtime/a.cpp")
        d = self.r.read("Engine/Runtime/a.cpp")
        self.assertEqual(d["path"], "Engine/Runtime/a.cpp")
        self.assertEqual(d["language"], "cpp")
        self.assertEqual(d["content"], "line1\nline2\n")

    def test_backslash_and_dotdot_inside(self):
        self.assertEqual(self.r.read("Engine\\Runtime\\a.cpp")["path"], "Engine/Runtime/a.cpp")
        self.assertEqual(self.r.read("Game/../Engine/Runtime/a.cpp")["path"], "Engine/Runtime/a.cpp")

    def test_dotdot_escape(self):
        self.assertCode("outside_root", self.r.resolve, "../outside/secret.txt")
        self.assertCode("outside_root", self.r.resolve, "Engine/../../outside/secret.txt")
        self.assertCode("outside_root", self.r.resolve, "..\\outside\\secret.txt")

    def test_absolute_outside(self):
        self.assertCode("outside_root", self.r.resolve, str(self.outside / "secret.txt"))
        if os.name != "nt":
            self.assertCode("outside_root", self.r.resolve, "/etc/passwd")

    def test_absolute_inside_allowed(self):
        p = self.r.resolve(str(self.root / "Engine/Runtime/a.cpp"))
        self.assertEqual(p.name, "a.cpp")

    def test_classify_path(self):
        cases = {
            "//server/share/x.cpp": "unc", "\\\\server\\share\\x.cpp": "unc", "///x": "unc",
            "C:foo.cpp": "drive_relative", "c:": "drive_relative",
            "C:/Windows/win.ini": "absolute", "C:\\Windows\\win.ini": "absolute",
            "/etc/passwd": "absolute", "\\etc\\passwd": "absolute",
            "Engine/a.cpp": "relative", "a\\b.cpp": "relative", "../x": "relative",
        }
        for p, kind in cases.items():
            self.assertEqual(classify_path(p), kind, p)

    def test_unc_and_drive_rejected_before_touching_fs(self):
        bad = ["//server/share/a.cpp", "\\\\server\\share\\a.cpp", "C:foo.cpp", "C:", "C:/Windows/win.ini",
               "C:\\Windows\\win.ini", "/etc/passwd", "../outside/secret.txt", str(self.root / ".." / "outside" / "secret.txt")]
        with mock.patch.object(Path, "resolve", side_effect=AssertionError("resolve() が呼ばれた")), \
             mock.patch.object(Path, "exists", side_effect=AssertionError("exists() が呼ばれた")), \
             mock.patch.object(Path, "stat", side_effect=AssertionError("stat() が呼ばれた")):
            for p in bad:
                with self.assertRaises(SourceError, msg=p) as cm:
                    self.r.resolve(p)
                self.assertEqual(cm.exception.code, "outside_root", p)

    def test_absolute_with_dotdot_back_inside(self):
        p = str(self.root / "Game" / ".." / "Engine" / "Runtime" / "a.cpp")
        self.assertEqual(self.r.read(p)["path"], "Engine/Runtime/a.cpp")

    def test_is_safe_relative(self):
        for ok in ["daisyui.css", "monaco/min/vs/loader.js", "a/../b.js", "./a.js"]:
            self.assertTrue(is_safe_relative(ok), ok)
        for bad in ["", "../x", "a/../../x", "/etc/passwd", "\\\\h\\s\\x", "//h/s/x", "C:x", "C:/x", "a\0b", "..\\x"]:
            self.assertFalse(is_safe_relative(bad), bad)

    def test_prefix_sibling_not_allowed(self):
        # /tmp/x/repo と /tmp/x/repo2 のような前方一致を誤許可しない
        sib = self.root.parent / "repo2"
        sib.mkdir()
        (sib / "f.txt").write_text("z")
        self.assertCode("outside_root", self.r.resolve, "../repo2/f.txt")

    @unittest.skipIf(os.name == "nt", "symlink 作成権限が不定")
    def test_symlink_escape(self):
        os.symlink(self.outside / "secret.txt", self.root / "link.txt")
        os.symlink(self.outside, self.root / "linkdir")
        self.assertCode("outside_root", self.r.resolve, "link.txt")
        self.assertCode("outside_root", self.r.resolve, "linkdir/secret.txt")
        # find_by_name も外を指すリンクは返さない
        self.assertEqual(self.r.find_by_name("secret.txt"), [])

    @unittest.skipIf(os.name == "nt", "symlink 作成権限が不定")
    def test_symlink_inside_ok(self):
        os.symlink(self.root / "Engine/Runtime/a.cpp", self.root / "alias.cpp")
        self.assertEqual(self.r.read("alias.cpp")["path"], "Engine/Runtime/a.cpp")

    def test_errors(self):
        self.assertCode("not_found", self.r.resolve, "nope.cpp")
        self.assertCode("not_file", self.r.resolve, "Engine")
        self.assertCode("invalid", self.r.resolve, "")
        self.assertCode("invalid", self.r.resolve, "a\0b")

    def test_no_root(self):
        r = SourceResolver(None)
        self.assertCode("no_root", r.resolve, "a.cpp")
        self.assertCode("no_root", r.find_by_name, "a.cpp")
        r2 = SourceResolver(self.root / "does_not_exist")
        self.assertCode("no_root", r2.resolve, "a.cpp")

    def test_too_large_and_binary(self):
        (self.root / "big.txt").write_bytes(b"a" * (MAX_SOURCE_BYTES + 1))
        (self.root / "bin.dat").write_bytes(b"ab\0cd")
        self.assertCode("too_large", self.r.read, "big.txt")
        self.assertCode("binary", self.r.read, "bin.dat")

    def test_decode_cp932(self):
        (self.root / "jp.cpp").write_bytes("// 日本語\n".encode("cp932"))
        self.assertEqual(self.r.read("jp.cpp")["content"], "// 日本語\n")

    def test_find_by_name(self):
        self.assertEqual(self.r.find_by_name("a.cpp"), ["Engine/Runtime/a.cpp"])
        self.assertEqual(self.r.find_by_name("SandboxUnit.cpp"),
                         ["Game/Unit/SandboxUnit.cpp", "Other/Unit/SandboxUnit.cpp"])
        self.assertEqual(self.r.find_by_name("sandboxunit.CPP"),
                         ["Game/Unit/SandboxUnit.cpp", "Other/Unit/SandboxUnit.cpp"])
        self.assertEqual(self.r.find_by_name("Hidden.cpp"), [])   # build 除外
        self.assertEqual(self.r.find_by_name("Hidden2.cpp"), [])  # node_modules 除外
        self.assertEqual(self.r.find_by_name("../outside/secret.txt"), [])
        self.assertEqual(self.r.find_by_name("Engine/Runtime/a.cpp"), [])
        self.assertEqual(self.r.find_by_name(""), [])

    def test_more_excluded_dirs(self):
        for d in ["Intermediate", ".vs", "out", "packages"]:
            (self.root / d).mkdir()
            (self.root / d / f"X_{d.strip('.')}.cpp").write_text("x")
        for d in ["Debug", "Release", "ThirdParty"]:
            (self.root / d).mkdir()
            (self.root / d / f"Y_{d}.cpp").write_text("y")
        r = SourceResolver(self.root)
        for d in ["Intermediate", "vs", "out", "packages"]:
            self.assertEqual(r.find_by_name(f"X_{d}.cpp"), [], d)
        for d in ["Debug", "Release", "ThirdParty"]:
            self.assertEqual(r.find_by_name(f"Y_{d}.cpp"), [f"{d}/Y_{d}.cpp"], d)

    def test_stale_index_rebuilt_when_not_found(self):
        now = [1000.0]
        self.r._clock = lambda: now[0]
        self.assertEqual(self.r.find_by_name("fresh.cpp"), [])
        (self.root / "fresh.cpp").write_text("x")
        now[0] += 29
        self.assertEqual(self.r.find_by_name("fresh.cpp"), [])          # まだ新しいので再構築しない
        now[0] += 2
        self.assertEqual(self.r.find_by_name("fresh.cpp"), ["fresh.cpp"])  # 30 秒超 -> 再構築して再検索
        now[0] += 1
        self.assertEqual(self.r.find_by_name("still_missing.cpp"), [])      # 再構築直後は再構築しない

    def test_index_cached_until_reset(self):
        self.assertEqual(self.r.find_by_name("late.cpp"), [])
        (self.root / "late.cpp").write_text("x")
        self.assertEqual(self.r.find_by_name("late.cpp"), [])
        self.r.reset_index()
        self.assertEqual(self.r.find_by_name("late.cpp"), ["late.cpp"])

    def test_language(self):
        for name, lang in [("a.cpp", "cpp"), ("a.H", "cpp"), ("a.hpp", "cpp"), ("a.inl", "cpp"),
                           ("a.hlsl", "cpp"), ("a.hlsli", "cpp"), ("a.py", "python"),
                           ("a.toml", "ini"), ("a.unknown", "plaintext"), ("noext", "plaintext")]:
            self.assertEqual(language_for(name), lang, name)


if __name__ == "__main__":
    unittest.main()
