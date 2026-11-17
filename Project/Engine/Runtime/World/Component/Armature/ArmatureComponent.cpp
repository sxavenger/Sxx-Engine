#include "ArmatureComponent.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* world
#include "../../Entity/EntityBehaviour.h"
#include "../../Entity/EntityStorage.h"
#include "ArmatureJointComponent.h"
#include "../Transform/TransformComponent.h"

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Logger/StackLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ArmatureComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ArmatureComponent::SetBone(const Assets::AssetHandle<Assets::Bone>& bone) {
	bone_ = bone; //!< boneを設定
}

RefPtr<const EntityBehaviour> ArmatureComponent::FindJointEntity(const std::string_view& name) const {
	RefPtr<const EntityBehaviour> result = ArmatureComponent::FindJointEntityRecursive(BaseComponent::GetOwner().Get(), name); //!< 自身のentityからjoint名を探索
	if (result == nullptr) {
		STACK_LOG_WARNING("World::ArmatureComponent | joint entity not found. name: {}", name);
	}

	return result;
}

void ArmatureComponent::Animate(const Assets::AssetHandle<Assets::AnimationSequence>& animation, TimePointd<TimeUnit::Second> time) {

	if (!bone_.HasUuid()) {
		STREAM_LOG_WARNING("World::ArmatureComponent | bone is not set.");
		return; //!< boneが設定されていない場合は処理を終了
	}

	//!< 自身のentityからjoint名を探索し, TransformComponentにtransformを設定
	ArmatureComponent::AnimateJointEntityRecursive(BaseComponent::GetOwner().Get(), animation.WaitGet()->GetDescription(), time); 
}

RefPtr<const EntityBehaviour> ArmatureComponent::FindJointEntityRecursive(const EntityBehaviour* entity, const std::string_view& name) {

	RefPtr<const ArmatureJointComponent> component = entity->GetComponent<ArmatureJointComponent>();
	if (component != nullptr && component->GetName() == name) {
		return entity; //!< joint名が一致した場合はentityを返す
	}

	for (const auto& handle : entity->GetChildren() | std::views::keys) {
		RefPtr<const EntityBehaviour> child = EntityStorage::GetInstance()->GetEntity(handle);
		RefPtr<const EntityBehaviour> found = ArmatureComponent::FindJointEntityRecursive(child, name); //!< 子のentityを再帰的に探索
		if (found != nullptr) {
			return found; //!< joint名が一致するentityが見つかった場合はそのentityを返す
		}
	}

	return nullptr; //!< joint名が一致するentityがなかった場合はnullptrを返す
}

void ArmatureComponent::AnimateJointEntityRecursive(EntityBehaviour* entity, const Assets::AnimationSequence::Description& animation, TimePointd<TimeUnit::Second> time) {

	RefPtr<const ArmatureJointComponent> component = entity->GetComponent<ArmatureJointComponent>();
	if (component != nullptr) {
		std::string_view name = component->GetName();
		if (animation.ContainsBone(name)) {
			Transform3d transform = animation.SampleBoneTransform(name, time);
			entity->GetComponent<TransformComponent>()->SetTransform(transform); //!< TransformComponentにtransformを設定
		}
	}

	for (const auto& handle : entity->GetChildren() | std::views::keys) {
		RefPtr<EntityBehaviour> child = EntityStorage::GetInstance()->GetEntity(handle);

		if (!child->HasComponent<ArmatureJointComponent>()) {
			continue;
		}

		ArmatureComponent::AnimateJointEntityRecursive(child, animation, time); //!< 子のentityを再帰的に探索
	}

}
