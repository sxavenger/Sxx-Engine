import os
import subprocess
import sys
import unittest
from pathlib import Path
from unittest import mock

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))

from sxxlog import picker  # noqa: E402


def cp(stdout="", returncode=0):
    return subprocess.CompletedProcess(args=[], returncode=returncode, stdout=stdout, stderr="")


class PickerTests(unittest.TestCase):
    def test_tk_selected_path_is_normalized_and_child_is_configured(self):
        with mock.patch.object(picker.subprocess, "run", return_value=cp('{"path": "/tmp//a/../b"}\n')) as run, \
             mock.patch.object(picker, "_is_windows", return_value=False):
            self.assertEqual(picker.pick_folder("/init", "T"), {"path": os.path.normpath("/tmp//a/../b")})
        args, kw = run.call_args
        self.assertEqual(args[0][:2], [sys.executable, "-c"])
        self.assertEqual(args[0][3:], ["/init", "T"])                 # initial, title は argv で渡す
        self.assertIn('reconfigure(encoding="utf-8")', args[0][2])    # 子側で UTF-8 出力
        self.assertIn('"-topmost"', args[0][2])
        self.assertIn("withdraw", args[0][2])
        self.assertEqual(kw["encoding"], "utf-8")                     # 親も UTF-8 で読む
        self.assertEqual(kw["stdin"], subprocess.DEVNULL)
        self.assertNotIn("timeout", kw)                               # ユーザー操作待ちなのでタイムアウト無し

    def test_unicode_path(self):
        with mock.patch.object(picker.subprocess, "run", return_value=cp('{"path": "C:/開発/エンジン"}')), \
             mock.patch.object(picker, "_is_windows", return_value=False):
            self.assertEqual(picker.pick_folder("", "")["path"], os.path.normpath("C:/開発/エンジン"))

    def test_cancel(self):
        with mock.patch.object(picker.subprocess, "run", return_value=cp('{"path": null}')) as run:
            self.assertEqual(picker.pick_folder("", "t"), {"path": None})
        self.assertEqual(run.call_count, 1)  # キャンセルではフォールバックしない

    def test_tk_unavailable_falls_back_to_powershell_on_windows(self):
        calls = []

        def fake_run(cmd, **kw):
            calls.append((cmd, kw))
            return cp('{"error": "no_tk"}') if len(calls) == 1 else cp("\ufeffD:\\Src\\Engine\r\n")

        with mock.patch.object(picker.subprocess, "run", side_effect=fake_run), \
             mock.patch.object(picker, "_is_windows", return_value=True):
            res = picker.pick_folder("D:\\Src", "ソース")
        self.assertEqual(res["path"], os.path.normpath("D:\\Src\\Engine"))
        cmd, kw = calls[1]
        self.assertEqual(cmd[0], "powershell.exe")
        for flag in ("-NoProfile", "-STA"):
            self.assertIn(flag, cmd)
        self.assertIn("FolderBrowserDialog", cmd[-1])
        self.assertIn("OutputEncoding", cmd[-1])
        self.assertEqual(kw["env"]["SXX_PICK_TITLE"], "ソース")
        self.assertEqual(kw["env"]["SXX_PICK_INITIAL"], "D:\\Src")
        self.assertEqual(kw["encoding"], "utf-8")

    def test_powershell_cancel(self):
        seq = [cp('{"error": "no_tk"}'), cp("")]
        with mock.patch.object(picker.subprocess, "run", side_effect=seq), \
             mock.patch.object(picker, "_is_windows", return_value=True):
            self.assertEqual(picker.pick_folder("", "t"), {"path": None})

    def test_both_unavailable(self):
        with mock.patch.object(picker.subprocess, "run", side_effect=[cp('{"error": "no_tk"}'), cp("", 1)]), \
             mock.patch.object(picker, "_is_windows", return_value=True):
            with self.assertRaises(picker.PickerUnavailable):
                picker.pick_folder("", "t")
        with mock.patch.object(picker.subprocess, "run", side_effect=FileNotFoundError), \
             mock.patch.object(picker, "_is_windows", return_value=True):
            with self.assertRaises(picker.PickerUnavailable):
                picker.pick_folder("", "t")

    def test_non_windows_without_tk_is_unavailable(self):
        with mock.patch.object(picker.subprocess, "run", return_value=cp('{"error": "no_tk"}')) as run, \
             mock.patch.object(picker, "_is_windows", return_value=False):
            with self.assertRaises(picker.PickerUnavailable):
                picker.pick_folder("", "t")
        self.assertEqual(run.call_count, 1)

    def test_tk_child_script_without_display_reports_no_tk(self):
        # 実プロセスで: tkinter が無い/ディスプレイが無い環境では no_tk か path のどちらかの JSON になり、落ちない
        env = dict(os.environ, DISPLAY="")
        p = subprocess.run([sys.executable, "-c", picker.TK_SCRIPT, "", "t"], capture_output=True, text=True,
                           encoding="utf-8", env=env, timeout=20, stdin=subprocess.DEVNULL)
        if os.name != "nt":
            self.assertIn('"error": "no_tk"', p.stdout)


if __name__ == "__main__":
    unittest.main()
