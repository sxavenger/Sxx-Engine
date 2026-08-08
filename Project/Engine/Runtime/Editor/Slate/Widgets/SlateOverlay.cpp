#include "SlateOverlay.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
// Overlay class methods
////////////////////////////////////////////////////////////////////////////////////////////

Slate::Overlay::OverlaySlot& Slate::Overlay::AddSlot() {
	slots_.emplace_back(std::make_unique<OverlaySlot>());
	return *slots_.back();
}

Vector2f Slate::Overlay::ComputeDesiredSize(float scale, ImGuiRenderer* renderer) const {
	Vector2f total = {};

	for (const auto& slot : slots_) {
		if (slot->widget == nullptr) {
			continue;
		}

		const Vector2f desired = slot->widget->ComputeDesiredSize(scale, renderer);
		slot->widget->CacheDesiredSize(desired);

		total = Math::Max(total, desired);
	}

	return total;
}

void Slate::Overlay::OnArrangeChildren(const Geometry& allotted, ArrangedChildren& out) const {
	for (const auto& slot : slots_) {
		if (slot->widget == nullptr) {
			continue;
		}

		const Vector2f desired = slot->widget->GetCachedDesiredSize();

		//!< Fill 以外は希望サイズで, 指定された位置へ寄せる.
		const float width  = (slot->hAlign == HAlign::Fill) ? allotted.localSize.x : desired.x;
		const float height = (slot->vAlign == VAlign::Fill) ? allotted.localSize.y : desired.y;

		float x = 0.0f;

		switch (slot->hAlign) {
			case HAlign::Right:  x = allotted.localSize.x - width;          break;
			case HAlign::Center: x = (allotted.localSize.x - width) * 0.5f; break;
			default: break; //!< Fill / Left.
		}

		float y = 0.0f;

		switch (slot->vAlign) {
			case VAlign::Bottom: y = allotted.localSize.y - height;          break;
			case VAlign::Center: y = (allotted.localSize.y - height) * 0.5f; break;
			default: break; //!< Fill / Top.
		}

		out.emplace_back(slot->widget, allotted.MakeChild({ x, y }, { width, height }));
	}
}

int32_t Slate::Overlay::OnPaint(const PaintArguments& arguments, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const {
	int32_t maxLayer = layer;

	ArrangedChildren arranged;
	OnArrangeChildren(geometry, arranged);

	//!< 追加順に描く(後のものが手前になる).
	for (const auto& child : arranged) {
		const int32_t childLayer = child.widget->OnPaint(arguments, child.geometry, renderer, maxLayer + 1);

		if (childLayer > maxLayer) {
			maxLayer = childLayer;
		}
	}

	return maxLayer;
}
