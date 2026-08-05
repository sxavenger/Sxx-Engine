-------------------------------------------------------------------------------------------
-- os
-------------------------------------------------------------------------------------------
-- 先頭で作業ディレクトリをprojectに変更
os.chdir(_SCRIPT_DIR .. "/../")

-------------------------------------------------------------------------------------------
-- Script c# solution
-------------------------------------------------------------------------------------------
project "Script"

	-- フォルダ指定
	location "Script"

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