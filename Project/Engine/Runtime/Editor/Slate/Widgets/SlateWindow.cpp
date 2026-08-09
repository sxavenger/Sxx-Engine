#include "SlateWindow.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "../Style/SlateStyle.h"
#include "../Renderer/SlateImGuiRenderer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Window class methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector2f Slate::Window::ComputeDesiredSize(float scale, ImGuiRenderer* renderer) const {
	if (pointer_ != nullptr) {
		Vector2f size = pointer_->ComputeDesiredSize(scale, renderer);
		pointer_->CacheDesiredSize(size);
	}

	//!< ルートはネイティブウィンドウのクライアントサイズに固定され、子の希望サイズには従わない.
	return clientSize_;
}

void Slate::Window::OnArrangeChildren(const Geometry& allotted, ArrangedChildren& out) const {
	if (pointer_ == nullptr || !pointer_->IsHitTestable()) {
		return;
	}

	out.emplace_back(pointer_, allotted);
}

int32_t Slate::Window::OnPaint(const PaintArguments& arguments, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const {
	if (renderer == nullptr) {
		return layer;
	}

	renderer->SetDrawTarget(ImGuiRenderer::DrawTarget::Background);
	renderer->DrawRect(geometry, Style::GetStyle().colors.background);

	int32_t maxLayer = layer;

	ArrangedChildren arranged;
	OnArrangeChildren(geometry, arranged);

	for (const auto& child : arranged) {
		maxLayer = std::max(maxLayer, child.widget->OnPaint(arguments, child.geometry, renderer, layer + 1));
	}

	return maxLayer;
}
