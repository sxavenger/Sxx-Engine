cd /d "%~dp0"

premake5 --version
premake5 --file=Sandbox_Premake5_vs2026.lua vs2026

echo Premake5 solution generation complete.

timeout /t 4 > nul
exit