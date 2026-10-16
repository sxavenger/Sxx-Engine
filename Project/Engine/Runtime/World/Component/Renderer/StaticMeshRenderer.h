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
class StaticMeshRenderer
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	StaticMeshRenderer(uint64_t handle, RefPtr<EntityBehaviour> entity) noexcept : BaseComponent(handle, entity) {}
	~StaticMeshRenderer() override = default;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Assets::AssetHandle<Assets::StaticMesh> mesh_          = std::nullopt; //!< 描画するメッシュ
	Assets::AssetHandle<Assets::SurfaceMaterial> material_ = std::nullopt; //!< 使用するマテリアル

	// TODO: 描画設定の追加

};

SXAVENGER_ENGINE_NAMESPACE_END
