#include "CaptureMainMenu.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "Slate/SlateIcon.h"
#include "SxGui/SxGui.h"

//* engine
#include <Runtime/Platform/WinApp.h>
#include <Runtime/Graphics/PixEvent.h>

//* imgui
#include <imgui.h>

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// CaptureMainMenu class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CaptureMainMenu::OnDraw() {
	if (ImGui::BeginMenu("Capture")) {

		if (SxGui::InputScalar<uint8_t>(std::format("{} Capture Frames", Slate::Icon::Stack), &captureFrameCount_, 1)) {
			captureFrameCount_ = std::clamp<uint8_t>(captureFrameCount_, 1, 10); //!< pixでの上限が10frameまでのため.
		}

		if (SxGui::ButtonStretchWidth(std::format("{} Capture Next Frames", Slate::Icon::ControlCamera))) {

			auto filepath = Platform::WinApp::GetSaveFilepath(
				L"PIXの保存先",
				std::filesystem::current_path(),
				{ L"PIXファイル", L"*.wpix;" },
				L".wpix"
			);

			if (filepath.has_value()) {
				Graphics::PixEvent::CaptureNextFrames(filepath.value(), captureFrameCount_);
			}
		}
		
		ImGui::EndMenu();
	}
}
