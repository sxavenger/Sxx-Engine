#include "SettingsPanel.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* SxGui
#include "SxGui/SxGui.h"

//* engine
#include <Runtime/Rendering/Renderer.h>

//* imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SettingsPanel class methods
////////////////////////////////////////////////////////////////////////////////////////////

SettingsPanel::SettingsPanel() {
	SetTitle(std::format("{} Settings", Slate::Icon::Settings));
	SetTabColor(Color4f::Blue());
}

void SettingsPanel::OnDraw() {

	if (SxGui::TreeNodeHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) { //!< Rendering設定

		Rendering::PassOption& option = Rendering::Renderer::GetOption();

		{
			SxGui::Text("Color Space");
			SxGui::Indent();

			SxGui::ComboEnum("Working Color Space", &option.workingColorSpace); //!< Working Color Spaceの設定

			SxGui::Unindent();
		}
		
		{
			SxGui::Text("Upscaler");
			SxGui::Indent();

			SxGui::ComboEnum("Upscaler", &option.upscaler);          //!< Upscalerの設定
			SxGui::ComboEnum("Upscaler Mode", &option.upscalerMode); //!< Upscaler Modeの設定

			SxGui::Unindent();
		}
		

		SxGui::TreePop();
	}

}
