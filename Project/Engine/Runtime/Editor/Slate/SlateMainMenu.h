#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateTypes.h"

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

	////////////////////////////////////////////////////////////////////////////////////////////
	// MainMenu class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ImGuiでmain menu barの中身を描くための基底クラス.
	//! @note 継承してOnDraw()にmenuを並べる. BeginMenuBar / EndMenuBar は基底が面倒を見るので,
	//!       OnDraw()の中は BeginMenu / MenuItem を書くだけでよい.
	//!
	//! ex.
	//! @code
	//! class FileMenu final
	//! 	: public Editor::Slate::MainMenu {
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
	//! 登録は SlateEditorUnit::AddMainMenu<FileMenu>() で行う.
	//!
	//! @note 所有はEditorBaseがstd::shared_ptrで持つ. (EditorBase::AddMainMenu)
	//!       menu barの1行は, EditorBaseが登録済みの全main menuのOnDraw()を順に呼んで描く.
	class MainMenu
		: public std::enable_shared_from_this<MainMenu> {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor / destructor *//

		MainMenu() noexcept          = default;
		virtual ~MainMenu() noexcept = default;

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

		//* operator [copy] <MainMenu> (delete) *//

		MainMenu(const MainMenu&)            = delete;
		MainMenu& operator=(const MainMenu&) = delete;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		float height_ = 0.0f; //!< 0以下ならImGuiMenuBarがStyleの値を使う.

	};

	using MainMenuPointer = std::shared_ptr<MainMenu>;

}

SXAVENGER_ENGINE_NAMESPACE_END
