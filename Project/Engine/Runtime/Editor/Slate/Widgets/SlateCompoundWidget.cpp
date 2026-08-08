#include "SlateCompoundWidget.h"
SXAVENGER_ENGINE_USING_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// CompoundWidget class
////////////////////////////////////////////////////////////////////////////////////////////

Vector2f Slate::CompoundWidget::ComputeDesiredSize(float scale, ImGuiRenderer* renderer) const {
	if (pointer_ == nullptr) {
		return {};
	}

	Vector2f s = pointer_->ComputeDesiredSize(scale, renderer);
	pointer_->CacheDesiredSize(s);
	return s;
}

void Slate::CompoundWidget::OnArrangeChildren(const Geometry& allotted, ArrangedChildren& out) const {
	if (pointer_ == nullptr || !pointer_->IsHitTestable()) {
		return;
	}

	Vector2f offset = { padding_.left, padding_.top };
	Vector2f size = {
		allotted.localSize.x - padding_.Horizontal(),
		allotted.localSize.y - padding_.Vertical()
	};

	out.emplace_back(pointer_, allotted.MakeChild(offset, size));
}

int32_t Slate::CompoundWidget::OnPaint(const PaintArguments& arguments, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const {
	int32_t maxLayer = layer;

	ArrangedChildren arranged;
	OnArrangeChildren(geometry, arranged);

	for (const auto& child : arranged) {
		maxLayer = std::max(maxLayer, child.widget->OnPaint(arguments, child.geometry, renderer, layer + 1));
	}

	return maxLayer;
}
