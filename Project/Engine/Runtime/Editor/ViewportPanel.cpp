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

	Vector2f region = SxGui::GetContentRegionAvail();

	if (pointer_ == nullptr) {

		static const std::string_view kDisplayText = "No Viewport";

		// 中心に "No Buffer" と表示する
		Vector2f textSize = SxGui::CalcTextSize(kDisplayText.data());

		SxGui::SetCursorPos((region - textSize) / 2.0f);
		SxGui::Text(kDisplayText.data());
		
		return; //!< バッファが無いので描画しない.
	}

	const Rendering::LightingBuffer& buffer = pointer_->GetInstance<Rendering::LightingBuffer>();
	const Rendering::RenderTargetTexture& texture = buffer.GetBuffer(Rendering::LightingBuffer::Layout::Direct);
	// TODO: 表示変更できるように設計

	SxGui::ImageAspectFit(texture.GetDescriptorSRV(), static_cast<Vector2f>(texture.GetResolution()), region);
}
