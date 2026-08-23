-------------------------------------------------------------------------------------------
-- os
-------------------------------------------------------------------------------------------
-- 先頭で作業ディレクトリをproject(Engine)に変更
os.chdir(_SCRIPT_DIR .. "/../")

-------------------------------------------------------------------------------------------
-- include
-------------------------------------------------------------------------------------------
include "Premake/DirectXTex_Solution_Premake5.lua"
include "Premake/ImGui_Solution_Premake5.lua"
include "Premake/meshoptimizer_Solution_Premake5.lua"
include "Premake/EngineScript_Solution_Premake5.lua"

-------------------------------------------------------------------------------------------
-- main c++ project
-------------------------------------------------------------------------------------------
project "SxavengerEngine"

	-- 出力先 (Generatedはリポジトリ直下なのでwks.location(Project/Engine)から2段上)
	targetdir "%{wks.location}/../Generated/outputs/%{cfg.buildcfg}/" -- 出力先(ConsoleまたはWindowsApplicationの場合)

	-- visual studioの設定 --
	toolset "v145"
	architecture "x64"

	-- projectの種類 --
	kind "StaticLib"

	-- debuggerの設定 --
	-- debuggertype "NativeWithManagedCore"

	-- 言語 --
	language "c++"
	cppdialect "c++20"

	-- ファイルの追加 --
	files {
		"%{prj.location}/*.cpp",
		"%{prj.location}/*.h",

		"%{prj.location}/Lib/**.cpp",
		"%{prj.location}/Lib/**.h",
		"%{prj.location}/Lib/**.hpp",

		"%{prj.location}/Runtime/**.cpp",
		"%{prj.location}/Runtime/**.h",
		"%{prj.location}/Runtime/**.hpp",

		"%{prj.location}/Unit/**.cpp",
		"%{prj.location}/Unit/**.h",
	}

	-- 追加include --
	includedirs {
		"%{prj.location}",

		-- 外部ライブラリ
		"%{prj.location}/Externals",

		"%{prj.location}/Externals/magic_enum", -- [magic_enum](https://github.com/Neargye/magic_enum.git)
		"%{prj.location}/Externals/nlohmann", -- [nlohmann](https://github.com/nlohmann/json.git)
		"%{prj.location}/Externals/tomlplusplus", -- [toml++](https://github.com/marzer/tomlplusplus.git)

		"%{prj.location}/Externals/DirectXTex/include", -- [DirectXTex](https://github.com/microsoft/DirectXTex.git)
		"%{prj.location}/Externals/dotnet/include", -- [dotnet](https://dotnet.microsoft.com)
		"%{prj.location}/Externals/PixEvents/include", -- [PixEvents](https://github.com/microsoft/PixEvents.git)
		"%{prj.location}/Externals/imgui", -- [imgui](https://github.com/ocornut/imgui.git)
		"%{prj.location}/Externals/assimp/include", -- [assimp](https://github.com/assimp/assimp.git)
	}

	-- 依存プロジェクト --
	dependson {
		"DirectXTex",
		"imgui",
		"EngineScript"
	}

	links {
		"DirectXTex",
		"imgui",
		"meshoptimizer",
	}

	-- ビルドオプション(共通) --
	warnings "High"
	multiprocessorcompile "On" -- 複数コアのでの並列コアコンパイル
	staticruntime "On"
	buildoptions { "/utf-8" }
	
	-- define定義(共通) --
	defines { '_PROFILE="$(Configuration)"', "NOMINMAX" }

	-- リンカー設定(共通) --
	linkoptions {
		"/WX",
		"/IGNORE:4099", -- [LNK4099](https://learn.microsoft.com/ja-jp/cpp/error-messages/tool-errors/linker-tools-warning-lnk4099)
	}

	--- 外部プログラムごとの設定 ---
	-- dxcompiler
	-- ビルド後イベント --
	postbuildcommands {
		'copy /Y "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll" "%{cfg.targetdir}"',
		'copy /Y "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll" "%{cfg.targetdir}"'
	}
	
	--- Hostfxr
	-- ビルド後イベント --
	postbuildcommands {
		-- host
    	'xcopy /E /Y /I "%{prj.location}/Externals/dotnet/host" "%{cfg.targetdir}/dotnet/host"',

    	-- shared
    	'xcopy /E /Y /I "%{prj.location}/Externals/dotnet/shared" "%{cfg.targetdir}/dotnet/shared"',
	}

	-- PixEvents
	filter "configurations:Debug"
		-- リンカー設定 --
		libdirs {
			"%{prj.location}/Externals/PixEvents/lib/Debug"
		}

		-- 依存ファイル --
		links {
			"WinPixEventRuntime",
		}

		-- ビルド後イベント --
		postbuildcommands {
			'copy /Y "%{prj.location}\\Externals\\PixEvents\\bin\\Debug\\WinPixEventRuntime.dll" "%{cfg.targetdir}"'
		}
	
	filter "configurations:Develop"
		-- リンカー設定 --
		libdirs {
			"%{prj.location}/Externals/PixEvents/lib/Release"
		}

		-- 依存ファイル --
		links {
			"WinPixEventRuntime",
		}

		-- ビルド後イベント --
		postbuildcommands {
			'copy /Y "%{prj.location}\\Externals\\PixEvents\\bin\\Release\\WinPixEventRuntime.dll" "%{cfg.targetdir}"'
		}

	-- assimp
	filter "configurations:Debug"
		-- リンカー設定 --
		libdirs {
			"%{prj.location}/Externals/assimp/lib/Debug"
		}

		-- 依存ファイル --
		links {
			"assimp-vc145-mtd",
			"zlibstaticd",
		}

	filter "configurations:Develop, Release"
		-- リンカー設定 --
		libdirs {
			"%{prj.location}/Externals/assimp/lib/Release"
		}

		-- 依存ファイル --
		links {
			"assimp-vc145-mt",
			"zlibstatic",
		}

	--- application構成ごとのビルドオプション設定 ---
	filter "kind:ConsoleApp"
    	defines { "CONSOLE" }
	
	--- project構成ごとのビルドオプション設定 ---
	-- Debug
	filter "configurations:Debug"
		-- ビルドオプション
		symbols "On"
		
		-- define定義
		defines { "DEVELOPMENT" }

	-- Develop
	filter "configurations:Develop"
		-- ビルドオプション
		optimize "On"

		-- define定義
		defines { "DEVELOPMENT" }

	-- Release
	filter "configurations:Release"
		-- ビルドオプション
		optimize "On"

		buildoptions {
			"/wd4100" -- [C4100](https://learn.microsoft.com/ja-jp/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4100)
		}

