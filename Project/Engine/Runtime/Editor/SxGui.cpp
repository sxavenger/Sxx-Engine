#include "SxGui.h"
SXAVENGER_ENGINE_USING_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

bool SxGui::ColorEdit3(const std::string_view& label, float col[3], ImGuiColorEditFlags flags) {
	return SxGui::ColorEdit3Impl(label.data(), col, flags);
}

bool SxGui::ColorEdit4(const std::string_view& label, float col[4], ImGuiColorEditFlags flags) {
	return SxGui::ColorEdit4Impl(label.data(), col, flags);
}

bool SxGui::ButtonStretch(const std::string_view& label) {
	ImVec2 region = ImGui::GetContentRegionAvail();
	return ImGui::Button(label.data(), { region.x, 0.0f });
}
