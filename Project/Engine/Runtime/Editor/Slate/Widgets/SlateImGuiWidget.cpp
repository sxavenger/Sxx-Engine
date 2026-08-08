#include "SlateImGuiWidget.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// ImGuiWidget class methods
////////////////////////////////////////////////////////////////////////////////////////////

int32_t Slate::ImGuiWidget::OnPaint(const PaintArguments& /*arguments*/, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const {
	if (renderer == nullptr || draw_ == nullptr) {
		return layer;
	}

	//!< サイズが 0 の領域では ImGui ウィンドウを開かない.
	//!< ImGuiRenderer::BeginRegion は WindowMinSize を (1, 1) にしているが, 0 のままだと
	//!< 空のウィンドウが残ってホバーを奪うため, 呼ぶ前に弾いておく.
	if (geometry.localSize.x <= 0.0f || geometry.localSize.y <= 0.0f) {
		return layer;
	}

	if (id_.empty()) {
		id_ = std::format("##slate_imgui_{:016X}", reinterpret_cast<uintptr_t>(this));
	}

	//!< BeginRegion / EndRegion は必ず対で呼ぶ.
	//!< BeginRegion が false を返した場合, ImGuiRenderer 側が ImGui::Begin を呼んだかどうかを
	//!< 覚えていて EndRegion が何もしないので, 戻り値によらず EndRegion を呼んでよい.
	//!< WindowRounding / WindowBorderSize / WindowMinSize の push / pop も BeginRegion 側の責務.
	if (renderer->BeginRegion(id_.c_str(), geometry)) {
		draw_();
	}

	renderer->EndRegion();

	return layer;
}
