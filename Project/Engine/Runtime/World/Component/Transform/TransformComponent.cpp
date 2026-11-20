#include "TransformComponent.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* world
#include "../../Entity/EntityBehaviour.h"
#include "../../Entity/EntityStorage.h"

////////////////////////////////////////////////////////////////////////////////////////////
// TransformComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TransformComponent::Update() {

	//!< transformからtransformationに変換
	world_ = Transform3d::ConvertTransformation(transform_);

	//!< Entityの親が存在する場合, 親のTransformComponentのtransformationを掛ける.
	if (BaseComponent::GetOwner()->HasParent()) {
		RefPtr<EntityBehaviour> parent = BaseComponent::GetOwner()->GetParent();

		if (parent->HasComponent<TransformComponent>()) {
			RefPtr<TransformComponent> component = parent->GetComponent<TransformComponent>();
			world_ *= component->GetWorld();
		}
	}
}
