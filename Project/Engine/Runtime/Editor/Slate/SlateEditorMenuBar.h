#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateTypes.h"
#include "Widgets/SlateWidget.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////
	// EditorMenuBar class
	////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ImGuiでmain menu barの中身を描くための基底クラス.
	//! @note 継承してOnDraw()にmenuを並べる. BeginMenuBar / EndMenuBar は基底が面倒を見るので,
	//!       OnDraw()の中は BeginMenu / MenuItem を書くだけでよい.
	//!
	//! ex.
	//! @code
	//! class MainMenuBar final
	//! 	: public Editor::Slate::EditorMenuBar {
	//! public:
	//! 	void OnDraw() override {
	//! 		if (ImGui::BeginMenu("File")) {
	//! 			if (ImGui::MenuItem("Save")) { ... }
	//! 			ImGui::EndMenu();
	//! 		}
	//! 	}
	//! };
	//! @endcode
	//!
	//! 登録は SlateEditorUnit::SetMenuBar<MainMenuBar>() で行う.
	//!
	//! @note このクラスは必ず std::shared_ptr で保持すること.
	//!       widgetへ渡すcallbackは weak_ptr を握るため, 破棄後に呼ばれても安全になる.
	//!       生ポインタやスタック上のインスタンスで使うと dangling する.
	class EditorMenuBar
		: public std::enable_shared_from_this<EditorMenuBar> {
	public:

		//=====================================================================================
		// public methods
		//=====================================================================================

		//* constructor / destructor *//

		EditorMenuBar() noexcept          = default;
		virtual ~EditorMenuBar() noexcept = default;

		//* menu bar methods *//

		//! @brief menu barの中身を描く.
		//! @note ImGuiのframe内かつ BeginMenuBar の内側で呼ばれる.
		//!       ImGui::BeginMenuBar / EndMenuBar / Begin / End は呼ばないこと.
		virtual void OnDraw() = 0;

		//! @brief 毎frame呼ばれる. 描画に依存しない更新に使う.
		virtual void OnUpdate() {}

		//* height option *//

		//! @brief menu barの高さ. 0以下ならStyleのmenuBarHeightを使う.
		void SetHeight(float height);

		float GetHeight() const { return height_; }

		//* widget option *//

		//! @brief このmenu barに対応するwidgetを取得する. 無ければ生成する.
		//! @note chromeへ組み込むのはこれ. 何度呼んでも同じ実体を返す.
		const WidgetPointer& GetWidget();

		bool HasWidget() const { return widget_ != nullptr; }

		//* operator [copy] <EditorMenuBar> (delete) *//

		EditorMenuBar(const EditorMenuBar&)            = delete;
		EditorMenuBar& operator=(const EditorMenuBar&) = delete;

	private:

		//=====================================================================================
		// private variables
		//=====================================================================================

		WidgetPointer widget_ = nullptr; //!< 遅延生成. GetWidget()で作る.

		float height_ = 0.0f; //!< 0以下ならImGuiMenuBarがStyleの値を使う.

		//=====================================================================================
		// private methods
		//=====================================================================================

		//! @brief ImGuiMenuBarを組み立てる.
		void CreateWidget();

	};

	using EditorMenuBarPointer = std::shared_ptr<EditorMenuBar>;

}

SXAVENGER_ENGINE_NAMESPACE_END
