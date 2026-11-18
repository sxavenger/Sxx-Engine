import importlib.util
import io
import sys
import tarfile
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
spec = importlib.util.spec_from_file_location("setup_vendor", ROOT / "tools" / "setup_vendor.py")
sv = importlib.util.module_from_spec(spec)
sys.modules["setup_vendor"] = sv
spec.loader.exec_module(sv)


def make_tgz(members):
    buf = io.BytesIO()
    with tarfile.open(fileobj=buf, mode="w:gz") as tar:
        for name, data, typ in members:
            ti = tarfile.TarInfo(name)
            if typ == "sym":
                ti.type = tarfile.SYMTYPE
                ti.linkname = "/etc/passwd"
                tar.addfile(ti)
            else:
                ti.size = len(data)
                tar.addfile(ti, io.BytesIO(data))
    return buf.getvalue()


class SetupVendorTests(unittest.TestCase):
    def test_extract_and_prefix(self):
        with tempfile.TemporaryDirectory() as d:
            tgz = make_tgz([("package/min/vs/loader.js", b"L", "f"), ("package/min/vs/a/b.js", b"B", "f"),
                            ("package/other.js", b"O", "f")])
            n = sv.extract(tgz, [("package/min/vs/", "monaco/min/vs/")], Path(d))
            self.assertEqual(n, 2)
            self.assertEqual((Path(d) / "monaco/min/vs/a/b.js").read_bytes(), b"B")
            self.assertFalse((Path(d) / "other.js").exists())

    def test_traversal_rejected(self):
        with tempfile.TemporaryDirectory() as d:
            tgz = make_tgz([("package/min/vs/../../../evil.js", b"x", "f")])
            with self.assertRaises(SystemExit):
                sv.extract(tgz, [("package/min/vs/", "monaco/min/vs/")], Path(d))
            self.assertFalse((Path(d).parent / "evil.js").exists())

    def test_symlink_ignored(self):
        with tempfile.TemporaryDirectory() as d:
            tgz = make_tgz([("package/min/vs/link", b"", "sym")])
            self.assertEqual(sv.extract(tgz, [("package/min/vs/", "m/")], Path(d)), 0)

    def test_pick_version(self):
        p = {"name": "x", "versions": {"5.0.0": 1, "5.10.0": 1, "5.9.0": 1, "6.0.0": 1, "5.11.0-beta.1": 1}}
        self.assertEqual(sv.pick_version(p, "5"), "5.10.0")
        self.assertEqual(sv.pick_version(p, "5.9.0"), "5.9.0")

    def test_integrity(self):
        import base64, hashlib
        data = b"abc"
        ok = {"integrity": "sha512-" + base64.b64encode(hashlib.sha512(data).digest()).decode()}
        sv.verify_integrity(data, ok)
        with self.assertRaises(SystemExit):
            sv.verify_integrity(b"abd", ok)


if __name__ == "__main__":
    unittest.main()
