-------------------------------------------------------------------------------------------
-- os
-------------------------------------------------------------------------------------------
-- 先頭で作業ディレクトリをprojectに変更
os.chdir(_SCRIPT_DIR .. "/../")

-------------------------------------------------------------------------------------------
-- include
-------------------------------------------------------------------------------------------
-- engine solution
include "Engine/Premake/SxavengerEngine_Solution_Premake5.lua"

-- script solution
include "Premake/Script_Solution_Premake5.lua"

-------------------------------------------------------------------------------------------
-- main c++ project
-------------------------------------------------------------------------------------------
project "Sandbox"

	-- visual studioの設定 --
	toolset "v145"
	architecture "x64"

	-- projectの種類 --
	kind "WindowedApp"

	-- debuggerの設定 --
	-- debuggertype "NativeWithManagedCore"

	-- 言語 --
	language "c++"
	cppdialect "c++20"

	-- ファイルの追加 --
	files {
		"%{prj.location}/*.cpp",
		"%{prj.location}/*.h",

		"%{prj.location}/Game/**.h",
		"%{prj.location}/Game/**.cpp",
	}

	-- 追加include --
	includedirs {
		"%{prj.location}",
		"%{prj.location}/Engine",
		
		"%{prj.location}/Engine/Externals/magic_enum", -- [magic_enum](https://github.com/Neargye/magic_enum.git)
		"%{prj.location}/Engine/Externals/nlohmann", -- [nlohmann](https://github.com/nlohmann/json.git)
		"%{prj.location}/Engine/Externals/tomlplusplus", -- [toml++](https://github.com/marzer/tomlplusplus.git)

		"%{prj.location}/Engine/Externals/DirectXTex/include", -- [DirectXTex](https://github.com/microsoft/DirectXTex.git)
		"%{prj.location}/Engine/Externals/dotnet/include", -- [dotnet](https://dotnet.microsoft.com)
		"%{prj.location}/Engine/Externals/PixEvents/include", -- [PixEvents](https://github.com/microsoft/PixEvents.git)
	}

	-- 依存プロジェクト --
	dependson {
		"Script",
		"SxavengerEngine"
	}

	links {
		"SxavengerEngine"
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

		-- ビルド済みライブラリ参照
		libdirs {
			"%{wks.location}/../Generated/bin/DirectXTex/Debug",
			"%{wks.location}/Engine/Externals/PixEvents/lib/Debug",
		}

		links {
			"WinPixEventRuntime",
		}

	-- Develop
	filter "configurations:Develop"
		-- ビルドオプション
		optimize "On"

		-- define定義
		defines { "DEVELOPMENT" }

		-- ビルド済みライブラリ参照
		libdirs {
			"%{wks.location}/../Generated/bin/DirectXTex/Release",
			"%{wks.location}/Engine/Externals/PixEvents/lib/Release",
		}

		links {
			"WinPixEventRuntime",
		}

	-- Release
	filter "configurations:Release"
		-- ビルドオプション
		optimize "On"

		buildoptions {
			"/wd4100" -- [C4100](https://learn.microsoft.com/ja-jp/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4100)
		}

		-- ビルド済みライブラリ参照
		libdirs {
			"%{wks.location}/../Generated/bin/DirectXTex/Release",
		}