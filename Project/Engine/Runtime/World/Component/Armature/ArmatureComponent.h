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

	////////////////////////////////////////////////////////////////////////////////////////////
	// Armature structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Armature final {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Joint structure
		////////////////////////////////////////////////////////////////////////////////////////////
		struct Joint final {
		public:

			//=========================================================================================
			// public variables
			//=========================================================================================

			std::string name;

			std::optional<uint32_t> parent; //!< 親のJointのindex, いなければnull

			Transform3d transform;
			Transformation3d skeletonSpaceMatrix;

		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Create(const Assets::Bone& bone);

		void Reset();

		void Animate(const Assets::AnimationSequence::Description& animation, TimePointd<TimeUnit::Second> time);

		// todo: IKの追加

		void UpdateTransformation();

		bool IsValid() const;

		bool Contains(const std::string_view& name) const { return jointMap.Contains(name); }

		const Joint& GetJoint(const std::string_view& name) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::vector<Joint> joints;
		StringHashMap<uint32_t> jointMap; //!< nameからjointのindexを取得するためのmap

	private:

		//=========================================================================================
		// private methods
		//=========================================================================================

		uint32_t RegisterJoint(const Joint& joint);

		void CreateJoint(const Assets::Bone::Node& node, const std::optional<uint32_t>& parent);

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ArmatureComponent(uint64_t handle, RefPtr<EntityBehaviour> owner) noexcept : BaseComponent(handle, owner) {}
	~ArmatureComponent() override = default;

	//* component option *//

	void SetBone(const Assets::AssetHandle<Assets::Bone>& bone);

	const Assets::AssetHandle<Assets::Bone>& GetBone() const { return bone_; }

	//* animation option *//

	void Animate(const Assets::AssetHandle<Assets::AnimationSequence>& animation, TimePointd<TimeUnit::Second> time);

	// TODO: animation方法等の検討.

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Assets::AssetHandle<Assets::Bone> bone_ = std::nullopt; //!< 使用するボーン構造

	// TODO: roop設定の追加

	//* runtime parameter *//

	Armature armature_;

};

SXAVENGER_ENGINE_NAMESPACE_END
