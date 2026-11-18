@echo off
setlocal
cd /d "%~dp0"

rem --- Python detection: py -3 first, then python. Actually run -c to reject the Store stub.
set "PYCMD="
py -3 -c "import sys" >nul 2>&1
if not errorlevel 1 set "PYCMD=py -3"
if defined PYCMD goto :found
python -c "import sys" >nul 2>&1
if not errorlevel 1 set "PYCMD=python"
if defined PYCMD goto :found

echo [エラー] Python が見つかりません。
echo.
echo Python 3.10 以上をインストールしてください。
echo   https://www.python.org/downloads/
echo インストール時に "Add python.exe to PATH" にチェックを入れてください。
echo Microsoft Store のスタブ python.exe しか無い場合も、上記から入れ直してください。
echo.
pause
exit /b 1

:found
rem --- Version check: 3.10 or later
%PYCMD% -c "import sys; sys.exit(0 if sys.version_info >= (3, 10) else 1)" >nul 2>&1
if errorlevel 1 goto :oldpython
goto :run

:oldpython
echo [エラー] Python のバージョンが古いです。3.10 以上が必要です。
echo.
echo 現在のバージョン:
%PYCMD% --version
echo.
echo https://www.python.org/downloads/ から新しい Python をインストールしてください。
echo.
pause
exit /b 1

:run
%PYCMD% "%~dp0sxxlogview.py" %*
set "RC=%errorlevel%"
if "%RC%"=="0" goto :end

echo.
echo [エラー] sxxlogview が異常終了しました。終了コード: %RC%
echo 上のメッセージを確認してください。
echo.
pause
exit /b %RC%

:end
endlocal
exit /b 0
