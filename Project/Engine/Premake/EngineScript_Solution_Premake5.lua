-------------------------------------------------------------------------------------------
-- os
-------------------------------------------------------------------------------------------
-- 先頭で作業ディレクトリをproject(Engine)に変更
os.chdir(_SCRIPT_DIR .. "/../")

-------------------------------------------------------------------------------------------
-- EngineScript solution
-------------------------------------------------------------------------------------------
project "EngineScript"

	-- フォルダ指定
	location "EngineScript"

	-- projectの種類
	kind "SharedLib"

	-- 言語
    language "C#"

	-- 使用する .NET バージョン
    dotnetframework "net10.0"

    files {
        "%{prj.location}/**.cs"
    }

    filter "configurations:Debug"
        optimize "Off"

	filter "configurations:Develop"
        optimize "On"

    filter "configurations:Release"
        optimize "On"