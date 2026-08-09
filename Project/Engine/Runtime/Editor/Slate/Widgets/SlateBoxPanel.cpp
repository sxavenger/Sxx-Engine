#include "SlateBoxPanel.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
// BoxPanel class methods
////////////////////////////////////////////////////////////////////////////////////////////

Slate::Slot& Slate::BoxPanel::AddSlot() {
	slots_.emplace_back(std::make_unique<Slot>());
	return *slots_.back();
}

Vector2f Slate::BoxPanel::ComputeDesiredSize(float scale, ImGuiRenderer* renderer) const {
	Vector2f total = {};

	for (const auto& slot : slots_) {
		if (slot->GetWidget() == nullptr) {
			continue;
		}

		const Vector2f desired = slot->GetWidget()->ComputeDesiredSize(scale, renderer);
		slot->GetWidget()->CacheDesiredSize(desired);

		if (isVertical_) {
			total.x = std::max(total.x, desired.x);
			total.y += desired.y;

		} else {
			total.x += desired.x;
			total.y = std::max(total.y, desired.y);
		}
	}

	return total;
}

void Slate::BoxPanel::OnArrangeChildren(const Geometry& allotted, ArrangedChildren& out) const {
	const float mainAxisTotal = isVertical_ ? allotted.localSize.y : allotted.localSize.x;

	//!< Auto スロットの合計サイズと Fill 比率の合計を求める.
	float usedByAuto = 0.0f;
	float fillSum    = 0.0f;

	for (const auto& slot : slots_) {
		if (slot->GetWidget() == nullptr) {
			continue;
		}

		if (slot->GetRule() == SizeRule::Fill) {
			fillSum += slot->GetFillRatio();

		} else {
			const Vector2f desired = slot->GetWidget()->GetCachedDesiredSize();
			usedByAuto += isVertical_ ? desired.y : desired.x;
		}
	}

	const float fillSpace = std::max(0.0f, mainAxisTotal - usedByAuto);

	//!< 主軸に沿って順に配置する.
	float cursor = 0.0f;

	for (const auto& slot : slots_) {
		if (slot->GetWidget() == nullptr) {
			continue;
		}

		const Vector2f desired = slot->GetWidget()->GetCachedDesiredSize();

		const float mainSize = (slot->GetRule() == SizeRule::Fill && fillSum > 0.0f)
			? fillSpace * (slot->GetFillRatio() / fillSum)
			: (isVertical_ ? desired.y : desired.x);

		//!< 交差軸は常に全幅(Fill). 余白は ImGui 側で付ける.
		const float crossFull = isVertical_ ? allotted.localSize.x : allotted.localSize.y;

		Vector2f offset = {};
		Vector2f size   = {};

		if (isVertical_) {
			offset = { 0.0f, cursor };
			size   = { crossFull, mainSize };

		} else {
			offset = { cursor, 0.0f };
			size   = { mainSize, crossFull };
		}

		out.emplace_back(slot->GetWidget(), allotted.MakeChild(offset, size));
		cursor += mainSize;
	}
}

int32_t Slate::BoxPanel::OnPaint(const PaintArguments& arguments, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const {
	int32_t maxLayer = layer;

	ArrangedChildren arranged;
	OnArrangeChildren(geometry, arranged);

	for (const auto& child : arranged) {
		maxLayer = std::max(maxLayer, child.widget->OnPaint(arguments, child.geometry, renderer, layer + 1));
	}

	return maxLayer;
}
