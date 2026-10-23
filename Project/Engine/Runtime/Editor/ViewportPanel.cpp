#include "ViewportPanel.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "SxGui/SxGui.h"

//* engine
#include <Runtime/Rendering/Buffer/LightingBuffer.h>

//* imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ViewportPanel class methods
////////////////////////////////////////////////////////////////////////////////////////////

ViewportPanel::ViewportPanel() {
	SetTitle(std::format("{} Viewport", Slate::Icon::Window));
	SetTabColor(Color4f::Blue());
}

void ViewportPanel::OnDraw() {

	ImVec2 region = ImGui::GetContentRegionAvail();

	if (pointer_ == nullptr) {

		static const std::string_view kDisplayText = "No Viewport";

		// 中心に "No Buffer" と表示する
		ImVec2 textSize = ImGui::CalcTextSize(kDisplayText.data());

		ImGui::SetCursorPosX((region.x - textSize.x) * 0.5f);
		ImGui::SetCursorPosY((region.y - textSize.y) * 0.5f);

		ImGui::Text(kDisplayText.data());
		
		return; //!< バッファが無いので描画しない.
	}

	const Rendering::LightingBuffer& buffer = pointer_->GetInstance<Rendering::LightingBuffer>();
	const Rendering::RenderTargetTexture& texture = buffer.GetBuffer(Rendering::LightingBuffer::Layout::Direct);

	ImVec2 resolution = { static_cast<float>(texture.GetResolution().x), static_cast<float>(texture.GetResolution().y) };

	float windowAspectRatio  = region.x / region.y;
	float textureAspectRatio = resolution.x / resolution.y;

	//!< 出力する画像サイズの設定

	ImVec2 displaySize = region;

	if (textureAspectRatio <= windowAspectRatio) {
		displaySize.x *= textureAspectRatio / windowAspectRatio;

	} else {
		displaySize.y *= windowAspectRatio / textureAspectRatio;
	}

	ImGui::Image(texture.GetDescriptorSRV().GetGPUHandle().ptr, displaySize);
}
