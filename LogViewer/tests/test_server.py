import http.client
import json
import sys
import tempfile
import threading
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))

from sxxlog.server import ConfigStore, make_server  # noqa: E402

LOG = ("Sxavenger Library File Logger\nprofile: Debug\nmain thread id: 1\ntimestamp: 2026-01-01 00:00:00\n"
       + "#" * 40 + "\n"
       "<Stack> [2026-01-01 00:00:00] [a/b.cpp:1] [main thread] [Debug] m1\n"
       "<Stack> [2026-01-01 00:00:01] [a/b.cpp:1] [main thread] [Debug] m2\n")


class ServerTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        base = Path(cls.tmp.name).resolve()
        cls.app = base / "app"
        (cls.app / "web").mkdir(parents=True)
        (cls.app / "web" / "index.html").write_text("<html>{{ASSET_DAISYUI_CSS}} {{ASSET_MODE}}</html>", encoding="utf-8")
        (cls.app / "vendor").mkdir()
        (cls.app / "vendor" / "ok.js").write_text("1", encoding="utf-8")
        (cls.app / "secret.txt").write_text("SECRET", encoding="utf-8")
        cls.logs = base / "logs"; cls.logs.mkdir()
        (cls.logs / "t.log").write_text(LOG, encoding="utf-8")
        (cls.logs / "notes.txt").write_text("x", encoding="utf-8")
        (base / "outside.log").write_text(LOG, encoding="utf-8")
        cls.src = base / "src"; (cls.src / "Engine").mkdir(parents=True)
        (cls.src / "Engine" / "a.cpp").write_text("int a;\n", encoding="utf-8")
        (cls.src / "Engine" / "b.cpp").write_text("int b;\n", encoding="utf-8")
        cfg = ConfigStore(cls.app / "config.json")
        cfg.update({"log_dir": str(cls.logs), "source_root": str(cls.src)})
        cls.srv = make_server(cls.app, cfg, 0)
        cls.port = cls.srv.port
        threading.Thread(target=cls.srv.serve_forever, daemon=True).start()

    @classmethod
    def tearDownClass(cls):
        cls.srv.shutdown(); cls.srv.server_close(); cls.tmp.cleanup()

    def req(self, method, path, body=None, headers=None):
        c = http.client.HTTPConnection("127.0.0.1", self.port, timeout=5)
        c.request(method, path, body=body, headers=headers or {})
        r = c.getresponse(); data = r.read(); c.close()
        return r.status, data

    def jget(self, path, **kw):
        st, data = self.req("GET", path, **kw)
        return st, json.loads(data)

    def test_index_cdn_mode_substitution(self):
        st, data = self.req("GET", "/")
        self.assertEqual(st, 200)
        self.assertIn(b"https://cdn.jsdelivr.net/npm/daisyui@5/daisyui.css", data)
        self.assertIn(b"cdn", data); self.assertNotIn(b"{{", data)

    def test_logs_and_log(self):
        st, d = self.jget("/api/logs")
        self.assertEqual([x["name"] for x in d["logs"]], ["t.log"])
        st, d = self.jget("/api/log?name=t.log&group=callsite")
        self.assertEqual(st, 200)
        self.assertEqual(d["entries"][0]["kind"], "stack_group")
        self.assertEqual(d["entries"][0]["count"], 2)
        st, d = self.jget("/api/log?name=t.log&group=none")
        self.assertEqual(len(d["entries"]), 2)
        st, _ = self.jget("/api/log?name=t.log&group=zzz"); self.assertEqual(st, 400)

    def test_log_name_traversal(self):
        for name in ["../outside.log", "..%2Foutside.log", "%2Fetc%2Fpasswd", "..%5Coutside.log", "..", ".",
                     "notes.txt", "", "a%00.log", "t.log%2F..%2Ft.log"]:
            st, _ = self.jget(f"/api/log?name={name}")
            self.assertIn(st, (400, 404), name)
        st, _ = self.jget("/api/log?name=nope.log"); self.assertEqual(st, 404)

    def test_source_api(self):
        st, d = self.jget("/api/source?path=Engine/a.cpp")
        self.assertEqual((st, d["language"], d["content"]), (200, "cpp", "int a;\n"))
        for p in ["../outside.log", "..%2F..%2Fetc%2Fpasswd", "%2Fetc%2Fpasswd", "..%5C..%5Cx"]:
            st, d = self.jget(f"/api/source?path={p}")
            self.assertEqual((st, d["error"]), (403, "outside_root"), p)
        st, d = self.jget("/api/source?path=Engine/zzz.cpp"); self.assertEqual(st, 404)
        st, d = self.jget("/api/find?name=a.cpp"); self.assertEqual(d["candidates"], ["Engine/a.cpp"])

    def test_vendor_traversal(self):
        self.assertEqual(self.req("GET", "/vendor/ok.js")[0], 200)
        for p in ["../secret.txt", "..%2Fsecret.txt", "%2e%2e/secret.txt", "..%5Csecret.txt", "ok.js/../../secret.txt",
                  "%5C%5Cserver%5Cshare%5Cx", "%2F%2Fserver%2Fshare%2Fx", "C:secret.txt", "C:%5Csecret.txt", "%2Fetc%2Fpasswd"]:
            st, data = self.req("GET", "/vendor/" + p)
            self.assertEqual(st, 404, p)
            self.assertNotIn(b"SECRET", data)

    def test_host_and_origin(self):
        st, _ = self.jget("/api/config", headers={"Host": "evil.example"}); self.assertEqual(st, 403)
        st, _ = self.jget("/api/config", headers={"Origin": "http://evil.example"}); self.assertEqual(st, 403)
        st, _ = self.req("POST", "/api/config", body=b'{"theme":"light"}', headers={"Origin": "http://evil.example"})
        self.assertEqual(st, 403)

    def test_config_validation(self):
        for body in [b'{"source_root":"/definitely/not/here"}', b'{"theme":"x"}', b'{"group_mode":"x"}', b"nope", b"[]"]:
            st, _ = self.req("POST", "/api/config", body=body, headers={"Content-Length": str(len(body))})
            self.assertEqual(st, 400, body)
        body = b'{"theme":"light"}'
        st, data = self.req("POST", "/api/config", body=body)
        self.assertEqual((st, json.loads(data)["theme"]), (200, "light"))

    def test_parse_endpoint(self):
        st, data = self.req("POST", "/api/parse?group=callsite", body=LOG.encode("utf-8"))
        d = json.loads(data)
        self.assertEqual((st, d["counts"]["logs"], d["entries"][0]["count"]), (200, 2, 2))


if __name__ == "__main__":
    unittest.main()


class PickFolderTests(ServerTests):
    """ServerTests の fixture を流用 (継承したテストは重複実行になるので skip する)。"""

    def pick(self, body=None, headers=None):
        raw = json.dumps(body if body is not None else {"initial": "", "title": "T"}).encode()
        st, data = self.req("POST", "/api/pick-folder", body=raw, headers=headers or {})
        return st, json.loads(data)

    def setUp(self):
        self.calls = []
        self.srv.picker = lambda initial, title: (self.calls.append((initial, title)) or {"path": "/chosen"})

    @classmethod
    def tearDownClass(cls):
        from sxxlog import picker as pm
        cls.srv.picker = pm.pick_folder
        super().tearDownClass()

    def test_ok(self):
        st, d = self.pick({"initial": str(self.logs), "title": "ログ"})
        self.assertEqual((st, d), (200, {"path": "/chosen"}))
        self.assertEqual(self.calls, [(str(self.logs), "ログ")])

    def test_initial_falls_back_to_source_root(self):
        self.pick({"initial": "/definitely/not/here"})
        self.assertEqual(self.calls[0][0], str(self.src))

    def test_cancel(self):
        self.srv.picker = lambda i, t: {"path": None}
        self.assertEqual(self.pick(), (200, {"path": None}))

    def test_unavailable_501(self):
        from sxxlog import picker as pm

        def boom(i, t):
            raise pm.PickerUnavailable()
        self.srv.picker = boom
        st, d = self.pick()
        self.assertEqual((st, d["error"]), (501, "unavailable"))
        self.assertIn("テキストで入力", d["message"])
        self.assertTrue(self.srv.pick_lock.acquire(blocking=False))   # ロックは解放されている
        self.srv.pick_lock.release()

    def test_concurrent_409(self):
        started, release = threading.Event(), threading.Event()

        def slow(i, t):
            started.set(); release.wait(10)
            return {"path": "/slow"}
        self.srv.picker = slow
        first = {}
        th = threading.Thread(target=lambda: first.update(r=self.pick()))
        th.start()
        self.assertTrue(started.wait(5))
        st, d = self.pick()
        self.assertEqual((st, d["error"]), (409, "busy"))
        release.set(); th.join(10)
        self.assertEqual(first["r"], (200, {"path": "/slow"}))
        self.assertEqual(self.pick()[0], 200)   # 終わればまた開ける

    def test_bad_origin_and_host_never_open_dialog(self):
        st, d = self.pick(headers={"Origin": "http://evil.example"})
        self.assertEqual((st, d["error"]), (403, "bad_origin"))
        st, d = self.pick(headers={"Host": "evil.example"})
        self.assertEqual((st, d["error"]), (403, "bad_host"))
        self.assertEqual(self.calls, [])

    def test_invalid_body(self):
        for raw in (b"nope", b"[]", b'{"initial": 5}'):
            st, _ = self.req("POST", "/api/pick-folder", body=raw)
            self.assertEqual(st, 400, raw)
        self.assertEqual(self.calls, [])

    def test_get_not_allowed(self):
        self.assertEqual(self.req("GET", "/api/pick-folder")[0], 404)


for _name in [n for n in dir(ServerTests) if n.startswith("test_")]:
    setattr(PickFolderTests, _name, None)   # 継承したテストは実行しない


class ConfigStoreTests(unittest.TestCase):
    def test_bom_config_is_read(self):
        with tempfile.TemporaryDirectory() as d:
            p = Path(d) / "config.json"
            p.write_bytes(b"\xef\xbb\xbf" + json.dumps({"theme": "light", "group_mode": "exact"}).encode("utf-8"))
            cfg = ConfigStore(p)
            self.assertTrue(cfg.loaded)
            self.assertEqual((cfg.snapshot()["theme"], cfg.snapshot()["group_mode"]), ("light", "exact"))

    def test_unwritable_config_warns_and_continues(self):
        with tempfile.TemporaryDirectory() as d:
            cfg = ConfigStore(Path(d) / "no_such_dir" / "config.json")   # 親が無く書き込めない
            import contextlib, io
            buf = io.StringIO()
            with contextlib.redirect_stderr(buf):
                out = cfg.update({"theme": "light"})
            self.assertEqual(out["theme"], "light")                      # 例外にならず反映される
            self.assertEqual(cfg.snapshot()["theme"], "light")
            self.assertIn("保存できません", cfg.warning)
            self.assertIn("警告", buf.getvalue())
            (Path(d) / "no_such_dir").mkdir()
            cfg.update({"theme": "dark"})
            self.assertEqual(cfg.warning, "")                            # 書けるようになれば解消
