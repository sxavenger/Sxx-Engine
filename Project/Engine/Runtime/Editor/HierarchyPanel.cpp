#include "HierarchyPanel.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* SxGui
#include "SxGui/SxGui.h"

//* engine
#include <Runtime/World/Entity/EntityStorage.h>

//* imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// HierarchyPanel class methods
////////////////////////////////////////////////////////////////////////////////////////////

HierarchyPanel::HierarchyPanel() {
	Slate::EditorPanel::SetTitle(std::format("{} Hierarchy", Slate::Icon::Hierarchy));
	Slate::EditorPanel::SetTabColor(Color4f::Blue());
}

void HierarchyPanel::OnDraw() {

	const World::EntityStorage* storage = World::EntityStorage::GetInstance();

	for (const auto& [handle, entity] : storage->storage_) {
		std::string text = std::format("{} | {}", handle.GetHandle(), entity->GetName());
		SxGui::Text(text);
	}
}
