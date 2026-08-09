#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateWidget.h"

//* slate [style]
#include "../Style/SlateStyle.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////////
	// ImGuiMenuBar class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ImGui のメニューバーを Slate のレイアウトに組み込むウィジェット.
	//! @note コールバックの中では ImGui::BeginMenu() / MenuItem() がそのまま使える.
	//!       ドロップダウンやショートカット表示も ImGui の機能がそのまま働く.
	//! @note コールバックの中で ImGui::BeginMenuBar / EndMenuBar を呼ぶ必要は無い(内部で行う).
	//! @note ImGui::BeginMainMenuBar() は使わない. あれはビューポート最上部(y = 0)に固定される
	//!       ため, 自前のタイトルバーと重なってしまう. このウィジェットは Slate のレイアウトが
	//!       決めた矩形に置かれる.
	//!
	//! 使用例:
	//!   Decl<ImGuiMenuBar> menuBar;
	//!   menuBar->Draw([]{
	//!       if (ImGui::BeginMenu("File")) {
	//!           if (ImGui::MenuItem("New", "Ctrl+N")) { ... }
	//!           ImGui::Separator();
	//!           if (ImGui::MenuItem("Exit")) { ... }
	//!           ImGui::EndMenu();
	//!       }
	//!   });
	class ImGuiMenuBar final
		: public Widget {
	public:

		//-----------------------------------------------------------------------------------------
		// using
		//-----------------------------------------------------------------------------------------

		using FDrawCallback = std::function<void()>;

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor / destructor *//

		//!< Decl<T> は std::make_shared<T>() するため, 既定コンストラクタが必要.
		ImGuiMenuBar() noexcept           = default;
		~ImGuiMenuBar() noexcept override = default;

		//* static parameter *//

		//! @brief メニューバーの高さ. スタイルは実行時に変更できるため関数で返す.
		static float Height() { return Style::GetStyle().metrics.menuBarHeight; }

		//* menu bar option *//

		//! @brief ImGui の描画コールバックを設定する.
		std::shared_ptr<ImGuiMenuBar> Draw(FDrawCallback callback) { draw_ = std::move(callback); return SharedThis(); }

		//! @brief 行の高さを上書きする.
		//! @note 0 以下の場合は Height() を都度参照する(実行時のスタイル変更に追従させるため).
		std::shared_ptr<ImGuiMenuBar> SetHeight(float height) { height_ = height; return SharedThis(); }

		//* widget option *//

		Vector2f ComputeDesiredSize(float /*scale*/, ImGuiRenderer* /*renderer*/) const override;

		int32_t OnPaint(const PaintArguments& /*arguments*/, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		FDrawCallback draw_ = nullptr;

		//!< 0 なら Height() を都度参照する.
		float height_ = 0.0f;

		//!< ImGui ウィンドウの ID. アドレスから一度だけ作る(毎フレーム変えると状態が引き継がれない).
		mutable std::string id_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		//! @brief フルエントなセッタで自身を shared_ptr<ImGuiMenuBar> として返すためのヘルパ.
		//! @note 移植元の SharedThis(this) に相当する. engine 側に同等の共通ヘルパは無い.
		std::shared_ptr<ImGuiMenuBar> SharedThis() { return std::static_pointer_cast<ImGuiMenuBar>(shared_from_this()); }

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
