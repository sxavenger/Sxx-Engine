#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateWidget.h"
#include "../Style/SlateStyle.h"

//* c++
#include <cstdint>
#include <functional>
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
	// TitleBar class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief OS のタイトルバー(非クライアント領域)を非表示にした代わりに, 自前で描画する
	//!        カスタムタイトルバー. タイトル文字列と 最小化 / 最大化・元に戻す / 閉じる ボタンを持つ.
	//! @note ドラッグ移動とリサイズは Win32 の WM_NCHITTEST 側で実現するため,
	//!       ボタン領域の判定を静的関数として公開する.
	class TitleBar final
		: public Widget {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Button enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief タイトルバーのボタン種別.
		//! @note PointerEvent::Button と名前が衝突するため, TitleBar のネスト型として定義する.
		enum class Button : int8_t {
			None     = -1,
			Minimize = 0,
			Maximize = 1,
			Close    = 2,
		};

		//-----------------------------------------------------------------------------------------
		// using
		//-----------------------------------------------------------------------------------------

		using FOnButton = std::function<void()>;

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* static parameter *//

		//! @brief タイトルバーの高さ. スタイルは実行時に変更できるため関数で返す.
		static float Height() { return Style::GetStyle().metrics.titleBarHeight; }

		static constexpr float ButtonsWidth() { return kButtonWidth * kButtonCount; }

		//* title option *//

		//! @brief title barに表示する名前を設定する.
		//! @note UTF-8として扱う. iconは文字列の一部として入れられる. (DockPanel::Titleと同じ)
		void SetTitle(std::string_view title) { title_ = title; }

		void SetMaximized(bool isMaximized) { maximized_ = isMaximized; }

		//! @brief ボタンのみモード. true の場合, 背景もタイトル文字も描かない
		//!        (タブバー側が背景を描く前提で, ウィンドウ操作ボタンだけを重ねる).
		void SetButtonsOnly(bool isButtonsOnly) { buttonsOnly_ = isButtonsOnly; }

		//* callback option *//

		void SetOnMinimize(FOnButton callback) { onMinimize_ = std::move(callback); }

		void SetOnMaximize(FOnButton callback) { onMaximize_ = std::move(callback); }

		void SetOnClose(FOnButton callback) { onClose_ = std::move(callback); }

		//* hit test option *//

		//! @brief クライアント座標 pos がどのボタン上かを返す.
		//! @note WM_NCHITTEST から呼ぶため, 描画結果に依存せず幾何的に計算する.
		//!       barHeight はボタン行の高さ. 統合モードではタブ行の高さを渡す.
		static Button HitTestButton(Vector2f pos, float clientWidth, float barHeight = Style::GetStyle().metrics.titleBarHeight);

		//* widget option *//

		Vector2f ComputeDesiredSize(float /*scale*/, ImGuiRenderer* /*renderer*/) const override;

		int32_t OnPaint(const PaintArguments& /*arguments*/, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const override;

		//* pointer option *//

		void OnMouseLeave() override { hovered_ = Button::None; }

		Reply OnMouseMove(const Geometry& geometry, const PointerEvent& event) override;

		Reply OnMouseButtonDown(const Geometry& geometry, const PointerEvent& event) override;

		Reply OnMouseButtonUp(const Geometry& geometry, const PointerEvent& event) override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* constant *//

		static constexpr float kButtonWidth   = 44.0f; //!< ボタン1つの幅.
		static constexpr int32_t kButtonCount = 3;

		//* parameter *//

		std::string title_;

		FOnButton onMinimize_ = nullptr;
		FOnButton onMaximize_ = nullptr;
		FOnButton onClose_    = nullptr;

		bool maximized_   = false;
		bool buttonsOnly_ = false; //!< true ならボタンだけ描く(背景・タイトルはタブバー側に任せる).

		//* runtime state *//

		mutable Button hovered_ = Button::None;
		mutable Button pressed_ = Button::None;

		//=========================================================================================
		// private methods
		//=========================================================================================

		//* titlebar helper methods *//

		static Vector2f ToLocal(const Geometry& geometry, Vector2f absolute) {
			return { absolute.x - geometry.absolutePosition.x, absolute.y - geometry.absolutePosition.y };
		}

		static Geometry ButtonGeometry(const Geometry& geometry, int32_t index);

		void PaintGlyph(ImGuiRenderer* renderer, const Geometry& bg, Button button) const;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
