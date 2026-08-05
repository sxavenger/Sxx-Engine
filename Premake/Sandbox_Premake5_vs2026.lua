-------------------------------------------------------------------------------------------
-- os
-------------------------------------------------------------------------------------------
-- 先頭で作業ディレクトリをprojectに変更
os.chdir(_SCRIPT_DIR .. "/../Project")

-------------------------------------------------------------------------------------------
-- main solution
-------------------------------------------------------------------------------------------
workspace "Sandbox"

	-- 構成プロパティの定義
	configurations { "Debug", "Develop", "Release" }

	-- プラットフォームの設定
	platforms { "x64" }
	
	-- スタートプロジェクトの設定
	startproject "Sandbox"

	-- 出力先の設定
	objdir "%{wks.location}/../Generated/obj/%{prj.name}/%{cfg.buildcfg}/" -- 中間ファイル

	filter "kind:ConsoleApp or kind:WindowedApp"
		targetdir "%{wks.location}/../Generated/outputs/%{cfg.buildcfg}/" -- 出力先(ConsoleまたはWindowsApplicationの場合)

	filter "kind:StaticLib or kind:SharedLib"
    	targetdir "%{wks.location}/../Generated/bin/%{prj.name}/%{cfg.buildcfg}/" -- 出力先(StaticLibの場合)

-------------------------------------------------------------------------------------------
-- solution
-------------------------------------------------------------------------------------------
include "Premake/Sandbox_Solution_Premake5.lua"