#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/AssetTraits.h"
#include "StaticMesh.h"
#include "StaticMeshBuilder.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssetTraits<StaticMesh> structure
////////////////////////////////////////////////////////////////////////////////////////////
template <>
struct AssetTraits<StaticMesh> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Build(std::shared_ptr<StaticMesh>& mesh) { StaticMeshBuilder::Build(mesh); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	static constexpr AssetType kType = AssetType::StaticMesh;

};

SXAVENGER_ENGINE_NAMESPACE_END
