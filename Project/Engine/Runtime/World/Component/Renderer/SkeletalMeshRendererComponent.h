#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Assets/Mesh/SkeletalMesh.h>
#include <Runtime/Assets/Material/SurfaceMaterial.h>
#include <Runtime/Assets/Handle/AssetHandle.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletalMeshRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class SkeletalMeshRendererComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkeletalMeshRendererComponent(uint64_t handle, RefPtr<EntityBehaviour> owner) noexcept : BaseComponent(handle, owner) {}
	~SkeletalMeshRendererComponent() override = default;

	//* component option *//

	void SetMesh(const Assets::AssetHandle<Assets::SkeletalMesh>& mesh) { mesh_ = mesh; }

	const Assets::AssetHandle<Assets::SkeletalMesh>& GetMesh() const { return mesh_; }

	void SetMaterial(const Assets::AssetHandle<Assets::SurfaceMaterial>& material) { material_ = material; }

	const Assets::AssetHandle<Assets::SurfaceMaterial>& GetMaterial() const { return material_; }

	//* joint option *//

	void SetJointName(const std::string_view& name) { jointName_ = name; }

	const std::string_view GetJointName() const { return jointName_; }

	RefPtr<const EntityBehaviour> FindJointEntity() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Assets::AssetHandle<Assets::SkeletalMesh> mesh_ = std::nullopt; //!< 描画するメッシュ
	Assets::AssetHandle<Assets::SurfaceMaterial> material_ = std::nullopt; //!< 使用するマテリアル

	std::string jointName_; //!< 描画するjoint名

	// TODO: 描画設定の追加

	//=========================================================================================
	// private methods
	//=========================================================================================

};

SXAVENGER_ENGINE_NAMESPACE_END
