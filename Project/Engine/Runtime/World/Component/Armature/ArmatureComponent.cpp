#include "ArmatureComponent.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [ArmatureComponent] Armature structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ArmatureComponent::Armature::Create(const Assets::Bone& bone) {
	CreateJoint(bone.GetDescription().root, std::nullopt);
}

void ArmatureComponent::Armature::Reset() {
	*this = Armature();
}

void ArmatureComponent::Armature::Animate(const Assets::AnimationSequence::Description& animation, TimePointd<TimeUnit::Second> time) {
	for (auto& joint : joints) { //!< 親が若いので(indexが子より親の方が小さいので)通常ループで処理可能

		if (!animation.boneAnimations.Contains(joint.name)) {
			continue; //!< jointに対応するboneAnimationがない場合は処理をスキップ
		}

		Transform3d transform = animation.SampleBoneTransform(joint.name, time);
		joint.transform = transform;
	}
}

void ArmatureComponent::Armature::UpdateTransformation() {
	for (auto& joint : joints) { //!< 親が若いので(indexが子より親の方が小さいので)通常ループで処理可能
		joint.skeletonSpaceMatrix = Transform3d::ConvertTransformation(joint.transform);

		if (joint.parent.has_value()) { //!< 親がいる場合
			uint32_t parent = joint.parent.value();
			joint.skeletonSpaceMatrix *= joints[parent].skeletonSpaceMatrix;
		}
	}
}

bool ArmatureComponent::Armature::IsValid() const {
	return !joints.empty(); //!< jointが一つもない場合はinvalid
}

const ArmatureComponent::Armature::Joint& ArmatureComponent::Armature::GetJoint(const std::string_view& name) const {
	STREAM_ASSERT(jointMap.Contains(name), "ArmatureComponent::Armature | joint not found. name: {}", name);

	uint32_t index = jointMap.At(name);
	return joints.at(index);
}

uint32_t ArmatureComponent::Armature::RegisterJoint(const Joint& joint) {

	uint32_t index = static_cast<uint32_t>(joints.size()); //!< joint登録順にindexを取得

	//!< jointを登録
	joints.emplace_back(joint);
	jointMap.Emplace(joint.name, index);

	//!< 登録したindexを返す
	return index;
}

void ArmatureComponent::Armature::CreateJoint(const Assets::Bone::Node& node, const std::optional<uint32_t>& parent) {

	Joint joint = {};
	joint.name   = node.name;

	joint.parent = parent;

	joint.transform           = node.transform;
	joint.skeletonSpaceMatrix = Transformation3d::Identity();

	uint32_t index = RegisterJoint(joint); //!< jointを登録

	for (const auto& child : node.children) {
		CreateJoint(child, index);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// ArmatureComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ArmatureComponent::SetBone(const Assets::AssetHandle<Assets::Bone>& bone) {
	bone_ = bone; //!< boneを設定
	armature_.Reset(); //!< armatureをリセット
}

void ArmatureComponent::Animate(const Assets::AssetHandle<Assets::AnimationSequence>& animation, TimePointd<TimeUnit::Second> time) {

	if (!bone_.HasUuid()) {
		STREAM_LOG_WARNING("World::ArmatureComponent | bone is not set.");
		return; //!< boneが設定されていない場合は処理を終了
	}

	if (!armature_.IsValid()) {
		armature_.Create(*bone_.WaitGet()); //!< armatureを作成
	}

	armature_.Animate(animation.WaitGet()->GetDescription(), time); //!< armatureをアニメーション
	armature_.UpdateTransformation(); //!< armatureの変換行列を更新
}
