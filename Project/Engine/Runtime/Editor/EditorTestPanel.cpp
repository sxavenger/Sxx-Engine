#include "EditorTestPanel.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EditorTestPanel class methods
////////////////////////////////////////////////////////////////////////////////////////////

EditorTestPanel::EditorTestPanel() {
	SetTitle(std::format("{} Test", Slate::Icon::Home));
	SetTabColor(Color4f::Magenta());
}

void EditorTestPanel::OnDraw() {
	ImGui::Text("Hello, EditorTestPanel!");
}
