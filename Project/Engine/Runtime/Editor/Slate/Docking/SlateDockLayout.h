#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "../SlateJson.h"

//* slate [docking]
#include "SlateDocking.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////////
	// DockLayout class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ドックレイアウト(タブ構成・分割比率・ウィンドウ配置)の保存と復元.
	//! @note 保存するのは「配置」だけで, パネルの中身は保存しない. 復元時はパネル ID(= タイトル文字列)
	//!       から PanelFactory が中身を作る. UE5 も同様にレイアウトとコンテンツを分けて扱う.
	//! @note 扱えるノードは Splitter と DockTabStack だけ. それ以外の型は null として書き出され,
	//!       復元されない(SlateDocking.h の走査規則と同じ. 表示用の chrome は渡さないこと).
	//!
	//! JSON 例:
	//! {
	//!   "version": 1,
	//!   "windows": [
	//!     {
	//!       "title": "Docking Test", "isMain": true,
	//!       "x": 100, "y": 100, "width": 1100, "height": 700, "maximized": false,
	//!       "root": {
	//!         "type": "splitter", "orientation": "horizontal", "ratio": 0.3,
	//!         "first":  { "type": "tabs", "active": 0, "panels": ["Panel A", "Panel B"] },
	//!         "second": { "type": "tabs", "active": 0, "panels": ["Panel C"] }
	//!       }
	//!     }
	//!   ]
	//! }
	class DockLayout final {
	public:

		//-----------------------------------------------------------------------------------------
		// using
		//-----------------------------------------------------------------------------------------

		//!< Slate 共通の JSON 型(SlateJson.h). 実体は nlohmann/json の薄いラッパ(json::node).
		using Json = Slate::Json;

		//! @brief パネル ID から中身付きのパネルを作る(アプリ側が用意する).
		//! @note 未知の ID には nullptr を返してよい(その場合そのタブは復元されない).
		using PanelFactory = std::function<DockPanelPointer(const std::string& id)>;

		////////////////////////////////////////////////////////////////////////////////////////////
		// WindowLayout structure
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief ウィンドウ1枚ぶんのレイアウト.
		struct WindowLayout {
		public:

			//=========================================================================================
			// public variables
			//=========================================================================================

			std::string title;
			bool isMain     = false;
			int32_t x       = 0;
			int32_t y       = 0;
			int32_t width   = 1280;
			int32_t height  = 720;
			bool maximized  = false;
			WidgetPointer root; //!< ドックツリー(Splitter / DockTabStack のみ).

		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* node option *//

		//! @brief ドックツリーを JSON へ変換する.
		//! @note Splitter / DockTabStack 以外(nullptr を含む)は null になる.
		static Json NodeToJson(const WidgetPointer& node);

		//! @brief JSON からドックツリーを復元する.
		//! @note 1枚も復元できなかったノードは nullptr を返す(レイアウトは壊さない).
		static WidgetPointer NodeFromJson(const Json& value, const PanelFactory& factory);

		//* layout option *//

		static std::string Serialize(const std::vector<WindowLayout>& windows);

		//! @brief JSON 文字列からレイアウトを復元する.
		//! @note 壊れたテキストでも例外を投げずに false を返す.
		static bool Deserialize(const std::string_view& jsonText, const PanelFactory& factory, std::vector<WindowLayout>& out);

		//* file option *//

		//! @brief レイアウトを UTF-8 のファイルへ書き出す.
		//! @note 親ディレクトリが無ければ作る. 失敗しても例外は投げず false を返す.
		static bool SaveToFile(const std::filesystem::path& filepath, const std::vector<WindowLayout>& windows);

		//! @brief レイアウトを UTF-8 のファイルから読み込む.
		//! @note ファイルが無い / 壊れている場合は false を返す(JsonFile::Load と違い assert しない).
		static bool LoadFromFile(const std::filesystem::path& filepath, const PanelFactory& factory, std::vector<WindowLayout>& out);

		//=========================================================================================
		// public variables
		//=========================================================================================

		//!< フォーマットのバージョン. 一致しない場合は読み込みを諦める.
		static constexpr int32_t kFormatVersion = 1;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
