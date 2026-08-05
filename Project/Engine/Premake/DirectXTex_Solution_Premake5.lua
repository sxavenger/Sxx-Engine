-------------------------------------------------------------------------------------------
-- os
-------------------------------------------------------------------------------------------
-- 先頭で作業ディレクトリをprojectに変更
os.chdir(_SCRIPT_DIR .. "/../")

-------------------------------------------------------------------------------------------
-- DirectXTex solution
-------------------------------------------------------------------------------------------
-- [DirectXTex](https://github.com/microsoft/DirectXTex.git)
-- DirectXTex_Desktop_2026_Win10.vcxprojを参照して作成.
project "DirectXTex"

	-- 構成プロパティの修正(DevelopをReleaseと同等に)
	removeconfigurations { "Develop" }
    configmap { ["Develop"] = "Release" }

	-- ファイル位置の設定 --
	location "Externals/DirectXTex"

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
		"%{prj.location}/include/DirectXTex/**.cpp",
		"%{prj.location}/include/DirectXTex/**.h",

		-- "%{prj.location}/Shaders/**.hlsl",
		-- "%{prj.location}/Shaders/**.cmd",
	}

	-- 追加include
	includedirs {
		-- include関係
		"%{prj.location}/include/DirectXTex",
		"%{prj.location}/include/DirectXTex/Common",

		-- shader関係
		"%{prj.location}/Shaders/Compiled",
	}

	-- ビルドオプション(共通)
	warnings "High"
	multiprocessorcompile "On" -- 複数コアのでの並列コアコンパイル
	staticruntime "On"
	floatingpoint "Fast"

	buildoptions {
		"/Zc:__cplusplus",
    	"/Zc:twoPhase-",
		"/utf-8",
	}

	-- define定義(共通)
	defines {
		"_UNICODE",
		"UNICODE",
		"WIN32",
		"_LIB",
		"_WIN32_WINNT=0x0A00",
		"_CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS"
	}

	-- ShaderCompile起動
	prebuildcommands {
   		'if not exist "%{prj.location}\\Shaders\\Compiled\\BC6HEncode_EncodeBlockCS.inc" ( ' ..
        'echo Compiling shaders... && ' ..
        'cd /d "%{prj.location}\\Shaders" && ' ..
        'CompileShaders.cmd' ..
    	' )'
    }

	cleancommands {
        "del /Q %{prj.location}/Shaders/Compiled\\*.inc",
        "del /Q %{prj.location}/Shaders/Compiled\\*.pdb"
    }

	--- 構成ごとの設定 ---
	filter "configurations:Debug"
		-- ビルドオプション
			symbols "On"
			fatalwarnings { "All" }
		
	filter "configurations:Release"
		-- ビルドオプション
		optimize "On"
