#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Assets/Mesh/StaticMesh.h>
#include <Runtime/Assets/Material/SurfaceMaterial.h>
#include <Runtime/Assets/Handle/AssetHandle.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// StaticMeshRenderer class
////////////////////////////////////////////////////////////////////////////////////////////
class StaticMeshRenderer final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	StaticMeshRenderer(uint64_t handle, RefPtr<EntityBehaviour> owner) noexcept : BaseComponent(handle, owner) {}
	~StaticMeshRenderer() override = default;

	//* component option *//

	void SetMesh(const Assets::AssetHandle<Assets::StaticMesh>& mesh) { mesh_ = mesh; }

	const Assets::AssetHandle<Assets::StaticMesh>& GetMesh() const { return mesh_; }

	void SetMaterial(const Assets::AssetHandle<Assets::SurfaceMaterial>& material) { material_ = material; }

	const Assets::AssetHandle<Assets::SurfaceMaterial>& GetMaterial() const { return material_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Assets::AssetHandle<Assets::StaticMesh> mesh_          = std::nullopt; //!< 描画するメッシュ
	Assets::AssetHandle<Assets::SurfaceMaterial> material_ = std::nullopt; //!< 使用するマテリアル

	// TODO: 描画設定の追加

};

SXAVENGER_ENGINE_NAMESPACE_END
