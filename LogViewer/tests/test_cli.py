import socket
import subprocess
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


class CliTests(unittest.TestCase):
    def test_port_in_use_exits_with_japanese_message(self):
        s = socket.socket()
        s.bind(("127.0.0.1", 0))
        s.listen(1)
        port = s.getsockname()[1]
        try:
            p = subprocess.run([sys.executable, str(ROOT / "sxxlogview.py"), "--no-browser", "--port", str(port),
                                ], capture_output=True, text=True, encoding="utf-8",
                               errors="replace", timeout=20)
        finally:
            s.close()
        self.assertEqual(p.returncode, 1)
        self.assertIn(f"ポート {port} を使用できません", p.stderr)
        self.assertNotIn("Traceback", p.stderr)


if __name__ == "__main__":
    unittest.main()
