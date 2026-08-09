#include "SlateTitleBar.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
// TitleBar class methods
////////////////////////////////////////////////////////////////////////////////////////////

Slate::TitleBar::Button Slate::TitleBar::HitTestButton(Vector2f pos, float clientWidth, float barHeight) {
	if (pos.y < 0.0f || pos.y >= barHeight) {
		return Button::None;
	}

	for (int32_t i = 0; i < kButtonCount; ++i) {
		const float x0 = clientWidth - kButtonWidth * (kButtonCount - i);
		const float x1 = x0 + kButtonWidth;

		if (pos.x >= x0 && pos.x < x1) {
			return static_cast<Button>(i);
		}
	}

	return Button::None;
}

Vector2f Slate::TitleBar::ComputeDesiredSize(float /*scale*/, ImGuiRenderer* /*renderer*/) const {
	//!< ボタンのみモードでは右上に必要な分だけを占める.
	if (buttonsOnly_) {
		return Vector2f{ ButtonsWidth(), Style::GetStyle().metrics.tabBarHeight };
	}

	return Vector2f{ 0.0f, Height() };
}

int32_t Slate::TitleBar::OnPaint(const PaintArguments& /*arguments*/, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const {

	//!< ボタンのみモードでは背景もタイトルも描かない(下のタブバーがそのまま見える).
	if (!buttonsOnly_) {

		//!< 背景と下端の区切り線. 色は都度参照する(静的変数に写すとスタイル変更に追従しない).
		renderer->DrawRect(geometry, Style::GetStyle().colors.titleBar);

		const float bottom = geometry.absolutePosition.y + geometry.localSize.y;
		renderer->DrawLine(
			{ geometry.absolutePosition.x, bottom },
			{ geometry.absolutePosition.x + geometry.localSize.x, bottom },
			Style::GetStyle().colors.border, 1.0f
		);

		//!< タイトル文字列(左寄せ・縦中央).
		if (!title_.empty()) {
			const Vector2f textSize = renderer->MeasureTextA(title_, Style::GetStyle().metrics.fontBody);

			Vector2f textPosition = geometry.absolutePosition;
			textPosition.x += Style::GetStyle().metrics.marginL;
			textPosition.y += (geometry.localSize.y - textSize.y) * 0.5f;

			renderer->DrawTextA(textPosition, title_, Style::GetStyle().colors.textDim, Style::GetStyle().metrics.fontBody);
		}
	}

	//!< ボタン3つ.
	for (int32_t i = 0; i < kButtonCount; ++i) {
		const Button button           = static_cast<Button>(i);
		const Geometry buttonGeometry = ButtonGeometry(geometry, i);

		//!< ホバー/押下時の背景(閉じるだけ赤系).
		if (hovered_ == button) {
			renderer->DrawRect(buttonGeometry, (button == Button::Close) ? Style::GetStyle().colors.close : Style::GetStyle().colors.buttonHover);
		}

		PaintGlyph(renderer, buttonGeometry, button);
	}

	return layer;
}

Slate::Reply Slate::TitleBar::OnMouseMove(const Geometry& geometry, const PointerEvent& event) {
	hovered_ = HitTestButton(ToLocal(geometry, event.screenPosition), geometry.localSize.x, geometry.localSize.y);
	return Reply::Handled();
}

Slate::Reply Slate::TitleBar::OnMouseButtonDown(const Geometry& geometry, const PointerEvent& event) {
	if (event.button != PointerEvent::Button::Left) {
		return Reply::Unhandled();
	}

	const Button hit = HitTestButton(ToLocal(geometry, event.screenPosition), geometry.localSize.x, geometry.localSize.y);

	if (hit == Button::None) {
		return Reply::Unhandled(); //!< 空き領域は Win32 が処理する.
	}

	pressed_ = hit;
	return Reply::Handled().CaptureMouse(shared_from_this());
}

Slate::Reply Slate::TitleBar::OnMouseButtonUp(const Geometry& geometry, const PointerEvent& event) {
	if (event.button != PointerEvent::Button::Left) {
		return Reply::Unhandled();
	}

	const Button pressed = pressed_;
	pressed_ = Button::None;

	Reply reply = Reply::Handled().ReleaseMouseCapture();

	if (pressed == Button::None) {
		return reply;
	}

	//!< 押下開始と同じボタン上で離した場合のみ発火する.
	if (HitTestButton(ToLocal(geometry, event.screenPosition), geometry.localSize.x, geometry.localSize.y) == pressed) {
		switch (pressed) {
			case Button::Minimize: if (onMinimize_) { onMinimize_(); } break;
			case Button::Maximize: if (onMaximize_) { onMaximize_(); } break;
			case Button::Close:    if (onClose_)    { onClose_();    } break;
			default:                                                  break;
		}
	}

	return reply;
}

Slate::Geometry Slate::TitleBar::ButtonGeometry(const Geometry& geometry, int32_t index) {
	const float x = geometry.absolutePosition.x + geometry.localSize.x - kButtonWidth * (kButtonCount - index);
	return Geometry{ { x, geometry.absolutePosition.y }, { kButtonWidth, geometry.localSize.y }, geometry.scale };
}

void Slate::TitleBar::PaintGlyph(ImGuiRenderer* renderer, const Geometry& bg, Button button) const {
	const float cx = bg.absolutePosition.x + bg.localSize.x * 0.5f;
	const float cy = bg.absolutePosition.y + bg.localSize.y * 0.5f;
	const float h  = 5.0f; //!< アイコンの半径相当.

	const Color4f fg = Style::GetStyle().colors.text;

	switch (button) {
		case Button::Minimize:
			renderer->DrawLine({ cx - h, cy }, { cx + h, cy }, fg, 1.0f);
			break;

		case Button::Maximize:
			if (maximized_) {
				//!< 元に戻す: 少しずらした2つの枠.
				renderer->DrawBorder(Geometry{ { cx - h, cy - h + 2.0f }, { h * 2.0f - 2.0f, h * 2.0f - 2.0f }, 1.0f }, fg, 1.0f);
				renderer->DrawBorder(Geometry{ { cx - h + 2.0f, cy - h }, { h * 2.0f - 2.0f, h * 2.0f - 2.0f }, 1.0f }, fg, 1.0f);

			} else {
				renderer->DrawBorder(Geometry{ { cx - h, cy - h }, { h * 2.0f, h * 2.0f }, 1.0f }, fg, 1.0f);
			}
			break;

		case Button::Close:
			renderer->DrawLine({ cx - h, cy - h }, { cx + h, cy + h }, fg, 1.2f);
			renderer->DrawLine({ cx - h, cy + h }, { cx + h, cy - h }, fg, 1.2f);
			break;

		default:
			break;
	}
}
