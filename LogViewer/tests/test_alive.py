import http.client
import socket
import sys
import tempfile
import threading
import time
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))

from sxxlog.server import ConfigStore, make_server  # noqa: E402

GRACE, PING = 0.3, 0.15


class AliveTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.app = Path(self.tmp.name)
        (self.app / "web").mkdir()
        (self.app / "web" / "index.html").write_text("x", encoding="utf-8")
        self.servers = []
        self.socks = []

    def tearDown(self):
        for s in self.socks:
            s.close()
        for srv, _ in self.servers:
            srv.server_close()
        self.tmp.cleanup()

    def start(self, auto_exit=True):
        srv = make_server(self.app, ConfigStore(self.app / "config.json"), 0,
                          auto_exit=auto_exit, exit_grace=GRACE, ping_interval=PING)
        done = threading.Event()
        threading.Thread(target=lambda: (srv.serve_forever(), done.set()), daemon=True).start()
        self.servers.append((srv, done))
        return srv, done

    def sse(self, srv, extra=""):
        s = socket.create_connection(("127.0.0.1", srv.port), timeout=5)
        s.sendall(f"GET /api/alive HTTP/1.1\r\nHost: 127.0.0.1:{srv.port}\r\n{extra}\r\n".encode())
        buf = b""
        while b": ok" not in buf:
            chunk = s.recv(4096)
            self.assertTrue(chunk, "接続がすぐ閉じられた")
            buf += chunk
        self.socks.append(s)
        return s, buf

    def wait_count(self, srv, n, t=3):
        end = time.time() + t
        while time.time() < end:
            if srv.alive.count == n:
                return True
            time.sleep(0.02)
        return False

    def test_sse_headers_and_ping(self):
        srv, _ = self.start(auto_exit=False)
        s, head = self.sse(srv)
        self.assertIn(b"200 OK", head)
        self.assertIn(b"text/event-stream", head)
        self.assertIn(b"no-cache", head)
        s.settimeout(3)
        buf = b""
        while b": ping" not in buf:
            buf += s.recv(4096)
        self.assertIn(b": ping\n\n", buf)

    def test_never_connected_does_not_exit(self):
        srv, done = self.start()
        self.assertFalse(done.wait(1.0))

    def test_exits_after_disconnect_and_grace(self):
        srv, done = self.start()
        s, _ = self.sse(srv)
        self.assertTrue(self.wait_count(srv, 1))
        self.assertFalse(done.wait(GRACE + 0.5))      # つながっている間は終了しない
        t0 = time.monotonic()
        s.close()
        self.assertTrue(done.wait(3))
        self.assertGreaterEqual(time.monotonic() - t0, GRACE - 0.05)   # 猶予を待ってから終了
        self.assertEqual(srv.exit_reason, "viewer_closed")

    def test_reconnect_within_grace_keeps_running(self):
        srv, done = self.start()
        s, _ = self.sse(srv)
        s.close()
        self.assertTrue(self.wait_count(srv, 0))
        s2, _ = self.sse(srv)                          # F5 相当: すぐ再接続
        self.assertFalse(done.wait(GRACE + 0.7))
        s2.close()
        self.assertTrue(done.wait(3))

    def test_two_clients_one_closed_keeps_running(self):
        srv, done = self.start()
        a, _ = self.sse(srv); b, _ = self.sse(srv)
        self.assertTrue(self.wait_count(srv, 2))
        a.close()
        self.assertTrue(self.wait_count(srv, 1))
        self.assertFalse(done.wait(GRACE + 0.7))
        b.close()
        self.assertTrue(done.wait(3))

    def test_keep_alive_never_exits(self):
        srv, done = self.start(auto_exit=False)
        s, _ = self.sse(srv)
        s.close()
        self.assertTrue(self.wait_count(srv, 0))
        self.assertFalse(done.wait(GRACE + 0.7))

    def test_bad_origin_and_host_rejected_and_not_counted(self):
        srv, _ = self.start()
        for hdr in ({"Origin": "http://evil.example"}, {"Host": "evil.example"}):
            c = http.client.HTTPConnection("127.0.0.1", srv.port, timeout=5)
            c.request("GET", "/api/alive", headers=hdr)
            r = c.getresponse(); r.read(); c.close()
            self.assertEqual(r.status, 403, hdr)
        self.assertEqual(srv.alive.count, 0)
        self.assertFalse(srv.alive.ever_connected)

    def test_other_apis_not_blocked_by_open_sse(self):
        srv, _ = self.start(auto_exit=False)
        for _ in range(4):
            self.sse(srv)
        t0 = time.monotonic()
        c = http.client.HTTPConnection("127.0.0.1", srv.port, timeout=5)
        c.request("GET", "/api/config"); r = c.getresponse(); r.read(); c.close()
        self.assertEqual(r.status, 200)
        self.assertLess(time.monotonic() - t0, 1.0)

    def test_shutdown_releases_sse_threads(self):
        srv, done = self.start()
        a, _ = self.sse(srv)
        b, _ = self.sse(srv)
        a.close()
        self.assertTrue(self.wait_count(srv, 1))
        srv.shutdown(); done.wait(3)
        srv.server_close()                              # stopping を立てる -> 残りの SSE ハンドラが抜ける
        b.settimeout(3)
        data = b"x"
        while data:
            data = b.recv(4096)                         # EOF になる (ぶら下がらない)
        self.assertTrue(self.wait_count(srv, 0))

    def test_head_not_supported(self):
        srv, _ = self.start(auto_exit=False)
        c = http.client.HTTPConnection("127.0.0.1", srv.port, timeout=5)
        c.request("HEAD", "/api/alive"); r = c.getresponse(); r.read(); c.close()
        self.assertEqual(r.status, 405)


if __name__ == "__main__":
    unittest.main()
