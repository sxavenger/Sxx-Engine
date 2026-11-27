#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Assets/Bone/Bone.h>
#include <Runtime/Assets/Animation/AnimationSequence.h>
#include <Runtime/Assets/Handle/AssetHandle.h>

//* lib
#include <Lib/Container/StringHashMap.h>
#include <Lib/Math/Transform/Transformation3d.h>
#include <Lib/Math/Transform/Transform3d.h>

//* c++
#include <string>
#include <optional>
#include <vector>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// ArmatureComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class ArmatureComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ArmatureComponent(uint64_t handle, RefPtr<EntityBehaviour> owner) noexcept : BaseComponent(handle, owner) {}
	~ArmatureComponent() override = default;

	//* component option *//

	void SetBone(const Assets::AssetHandle<Assets::Bone>& bone);

	const Assets::AssetHandle<Assets::Bone>& GetBone() const { return bone_; }

	//* joint option *//

	RefPtr<const EntityBehaviour> FindJointEntity(const std::string_view& name) const;

	//* animation option *//

	void Animate(const Assets::AssetHandle<Assets::AnimationSequence>& animation, TimePointd<TimeUnit::Second> time);
	// TODO: animation方法等の検討.

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Assets::AssetHandle<Assets::Bone> bone_ = std::nullopt; //!< 使用するボーン構造

	// TODO: roop設定の追加

	//=========================================================================================
	// private variables
	//=========================================================================================

	static RefPtr<const EntityBehaviour> FindJointEntityRecursive(const EntityBehaviour* entity, const std::string_view& name);

	static void AnimateJointEntityRecursive(EntityBehaviour* entity, const Assets::AnimationSequence::Description& animation, TimePointd<TimeUnit::Second> time);

};

SXAVENGER_ENGINE_NAMESPACE_END
