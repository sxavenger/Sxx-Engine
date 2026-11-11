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
		pointer_->Prepass(scale, renderer);
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

	return PaintArrangedChildren(arguments, geometry, renderer, layer);
}
