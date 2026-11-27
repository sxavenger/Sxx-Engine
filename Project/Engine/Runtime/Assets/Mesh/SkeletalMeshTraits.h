#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/AssetTraits.h"
#include "SkeletalMesh.h"
#include "SkeletalMeshBuilder.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssetTraits<SkeletalMesh> structure
////////////////////////////////////////////////////////////////////////////////////////////
template <>
struct AssetTraits<SkeletalMesh> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Build(std::shared_ptr<SkeletalMesh>& mesh) { SkeletalMeshBuilder::Build(mesh); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	static constexpr AssetType kType = AssetType::SkeletalMesh;

};

SXAVENGER_ENGINE_NAMESPACE_END
