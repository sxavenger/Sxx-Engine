#include "SkeletalMeshRendererComponent.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* world
#include "../../Entity/EntityBehaviour.h"
#include "../../Entity/EntityStorage.h"
#include "../Armature/ArmatureComponent.h"

//* lib
#include <Lib/Logger/StackLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletalMeshRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

RefPtr<const EntityBehaviour> SkeletalMeshRendererComponent::FindJointEntity() const {
	static const std::function<RefPtr<const ArmatureComponent>(const EntityBehaviour*)> FindArmatureEntityRecursive =
		[](const EntityBehaviour* entity) -> RefPtr<const ArmatureComponent> {

		if (entity->HasComponent<ArmatureComponent>()) {
			return entity->GetComponent<ArmatureComponent>(); //!< ArmatureComponentを返す
		}

		if (entity->HasParent()) {
			RefPtr<const EntityBehaviour> parent = entity->GetParent();
			return FindArmatureEntityRecursive(parent); //!< 親のentityを再帰的に探索
		}

		return nullptr;
	};

	RefPtr<const ArmatureComponent> armature = FindArmatureEntityRecursive(BaseComponent::GetOwner().Get());
	if (armature == nullptr) {
		STACK_LOG_WARNING("World::SkeletalMeshRendererComponent | armature component not found.");
		return nullptr; //!< ArmatureComponentが見つからない場合はnullptrを返す
	}

	return armature->FindJointEntity(jointName_); //!< ArmatureComponentからjoint名を探索
}
