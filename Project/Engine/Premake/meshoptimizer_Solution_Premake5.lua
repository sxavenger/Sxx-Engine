-------------------------------------------------------------------------------------------
-- os
-------------------------------------------------------------------------------------------
-- 先頭で作業ディレクトリをprojectに変更
os.chdir(_SCRIPT_DIR .. "/../")

-------------------------------------------------------------------------------------------
-- meshoptimizer solution
-------------------------------------------------------------------------------------------
-- [meshoptimizer](https://github.com/zeux/meshoptimizer.git)
project "meshoptimizer"

	-- 構成プロパティの修正(DevelopをReleaseと同等に)
	removeconfigurations { "Develop" }
	configmap { ["Develop"] = "Release" }

	-- ファイル位置の設定 --
	location "Externals/meshoptimizer"

	-- visual studioの設定 --
	toolset "v145"
	architecture "x64"

	-- projectの種類 --
	kind "StaticLib"

	-- 言語 --
	language "c++"
	cppdialect "c++20"

	-- ファイルの追加 --
	files {
		"%{prj.location}/**.cpp",
		"%{prj.location}/**.h",
	}

	-- 追加include
	includedirs {
		"%{prj.location}",
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
	
	--- project構成ごとのビルドオプション設定 ---
	-- Debug
	filter "configurations:Debug"
		-- ビルドオプション
		symbols "On"

		-- define定義
		defines { "DEVELOPMENT" }

	-- Release
	filter "configurations:Release"
		-- ビルドオプション
		optimize "On"

		buildoptions {
			"/wd4100" -- [C4100](https://learn.microsoft.com/ja-jp/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4100)
		}