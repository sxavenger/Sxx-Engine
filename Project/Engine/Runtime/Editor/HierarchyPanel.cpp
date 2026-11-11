#include "HierarchyPanel.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "Slate/SlateEditorBase.h"

//* SxGui
#include "SxGui/SxGui.h"

//* editor
#include "InspectorPanel.h"

//* engine
#include <Runtime/World/Entity/EntityStorage.h>

//* imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// (anonymous namespace)
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	void HierarchyNode(const World::EntityHandle& handle, World::EntityHandle& selectedHandle) {

		World::EntityBehaviour* entity = World::EntityStorage::GetInstance()->GetEntity(handle);

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		Slate::Icon icon  = Slate::Icon::Cube; // TODO: Mobilityによるicon変更
		std::string label = std::format("{} {} ## {}", icon, entity->GetName(), handle.GetHandle());

		bool selected = (handle == selectedHandle);
		bool active   = entity->IsActive();
		bool root     = !entity->HasChildren();

		if (selected) {
			flags |= ImGuiTreeNodeFlags_Selected; //!< 選択状態のEntityBehaviourは選択色で表示する.
		}

		if (root) {
			flags |= ImGuiTreeNodeFlags_Leaf; //!< 子を持たないEntityBehaviourはleafとして表示する.
		}

		if (!active) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled)); //!< 非アクティブなEntityBehaviourはdisabled色で表示する.
		}

		bool open = SxGui::TreeNodeEx(label.c_str(), flags);

		if (SxGui::IsItemClicked()) {
			selectedHandle = handle; //!< クリックされたEntityBehaviourを選択状態にする.
		}

		if (!active) {
			ImGui::PopStyleColor();
		}

		if (open) {
			for (const auto& child : entity->GetChildren() | std::views::keys) {
				HierarchyNode(child, selectedHandle);
			}

			ImGui::TreePop();
		}
		
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// HierarchyPanel class methods
////////////////////////////////////////////////////////////////////////////////////////////

HierarchyPanel::HierarchyPanel() {
	Slate::EditorPanel::SetTitle(std::format("{} Hierarchy", Slate::Icon::Hierarchy));
	Slate::EditorPanel::SetTabColor(Color4f::Blue());
}

void HierarchyPanel::OnDraw() {

	InspectorPanel* inspector = EditorPanel::GetEditor()->GetPanel<InspectorPanel>().get();

	const World::EntityStorage::Storage& storage = World::EntityStorage::GetInstance()->GetStorage();

	World::EntityHandle selected = std::nullopt;

	if (inspector != nullptr) {
		//!< InspectorPanelが存在する場合はInspectorPanelの選択状態を取得する.
		selected = inspector->GetSelectedEntity();
	}

	//!< rootのEntityBehaviourを列挙する. (親を持たないEntityBehaviourがrootになる)
	for (const auto& [handle, entity] : storage | std::views::filter([](const auto& v) { return !v.second->HasParent(); })) {
		HierarchyNode(handle, selected);
	}

	if (inspector != nullptr) {
		//!< InspectorPanelが存在する場合はInspectorPanelの選択状態を更新する.
		inspector->SetSelectedEntity(selected);
	}
	
}
