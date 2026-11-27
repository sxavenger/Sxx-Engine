#include "InspectorPanel.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* SxGui
#include "SxGui/SxGui.h"

//* engine
#include <Runtime/World/Component/Transform/TransformComponent.h>
#include <Runtime/World/Entity/EntityBehaviour.h>
#include <Runtime/World/Entity/EntityFactory.h>

//* imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// InspectorPanel class methods
////////////////////////////////////////////////////////////////////////////////////////////

InspectorPanel::InspectorPanel() {
	SetTitle(std::format("{} Inspector", Slate::Icon::Cube));
	SetTabColor(Color4f::Blue());
}

void InspectorPanel::OnDraw() {

	if (!handle_.HasHandle()) {
		SxGui::TextCenter("No Entity Selected", SxGui::GetContentRegionAvail());
		return; //!< EntityHandleが有効でない場合は何も描画しない
	}

	RefPtr<World::EntityBehaviour> entity = World::EntityFactory::GetEntity(handle_);

	SxGui::Text(std::format("Entity Name: {}", entity->GetName()));

	if (auto component = entity->GetComponent<World::TransformComponent>()) {
		auto& transform = component->GetTransform();

		//!< Translationの編集
		SxGui::DragVectorN<float, 3>("Translation", &transform.translation.x, 0.01f);

		//!< Rotationの編集
		{

			//!< eulerの表示
			Vector3f prev = Quaternionf::ToEuler(transform.rotation);
			Vector3f edit = prev;

			if (SxGui::DragVectorN<float, 3>("Rotation", &edit.x, 0.01f)) {
				//!< eulerをQuaternionに変換して格納
				Vector3f delta = edit - prev;

				Vector3<Quaternionf> q = {
					Quaternionf::AxisAngle(Vector3f::UnitX(), delta.x),
					Quaternionf::AxisAngle(Vector3f::UnitY(), delta.y),
					Quaternionf::AxisAngle(Vector3f::UnitZ(), delta.z)
				};

				transform.rotation *= q.x * q.y * q.z;
			}
		}

		//!< Scaleの編集
		SxGui::DragVectorN<float, 3>("Scale", &transform.scale.x, 0.01f);
	}
}
