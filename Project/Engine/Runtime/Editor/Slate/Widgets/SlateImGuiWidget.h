#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateWidget.h"

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
	// ImGuiWidget class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Slate のウィジェットツリーの中に ImGui のウィジェットを埋め込むための箱.
	//! @note コールバックの中では ImGui::Button() / SliderFloat() / InputText() など通常の
	//!       ImGui API がそのまま使える. 位置とサイズは Slate のレイアウトが決め,
	//!       その矩形に装飾なしの ImGui ウィンドウを重ねる.
	//! @note コールバックの中で ImGui::Begin / End を呼ぶ必要は無い(内部で行う).
	//!
	//! 使用例:
	//!   Decl<ImGuiWidget> widget;
	//!   widget->Draw([]{
	//!       ImGui::Text("Hello");
	//!       if (ImGui::Button("Apply")) { ... }
	//!   });
	class ImGuiWidget final
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
		ImGuiWidget() noexcept           = default;
		~ImGuiWidget() noexcept override = default;

		//* imgui widget option *//

		//! @brief ImGui の描画コールバックを設定する.
		std::shared_ptr<ImGuiWidget> Draw(FDrawCallback callback) { draw_ = std::move(callback); return SharedThis(); }

		//! @brief 希望サイズ. 親が Auto スロットに置く場合に使う.
		std::shared_ptr<ImGuiWidget> DesiredSize(const Vector2f& size) { desiredSize_ = size; return SharedThis(); }

		//* widget option *//

		Vector2f ComputeDesiredSize(float /*scale*/, ImGuiRenderer* /*renderer*/) const override { return desiredSize_; }

		int32_t OnPaint(const PaintArguments& /*arguments*/, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const override;

		//* pointer option *//

		//!< 入力ハンドラは持たない. ImGui のウィジェットは ImGui 自身が処理するため,
		//!< この矩形上のクリックは常に Unhandled として祖先(タブバー・仕切り)へバブルするだけで
		//!< 何も奪わない. Slate へ配送するかどうかの判定は ImGuiRenderer::IsInteracting() 側で行う.

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		FDrawCallback draw_ = nullptr;

		Vector2f desiredSize_ = { 0.0f, 120.0f };

		//!< ImGui ウィンドウの ID. アドレスから一度だけ作る(毎フレーム変えると状態が引き継がれない).
		mutable std::string id_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		//! @brief フルエントなセッタで自身を shared_ptr<ImGuiWidget> として返すためのヘルパ.
		//! @note 移植元の SharedThis(this) に相当する. engine 側に同等の共通ヘルパは無い.
		std::shared_ptr<ImGuiWidget> SharedThis() { return std::static_pointer_cast<ImGuiWidget>(shared_from_this()); }

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
