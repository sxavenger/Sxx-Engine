#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/AssetTraits.h"
#include "SurfaceMaterial.h"
#include "SurfaceMaterialBuilder.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssetTraits<SurfaceMaterial> structure
////////////////////////////////////////////////////////////////////////////////////////////
template <>
struct AssetTraits<SurfaceMaterial> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Build(std::shared_ptr<SurfaceMaterial>& material) { SurfaceMaterialBuilder::Build(material); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	static constexpr AssetType kType = AssetType::SurfaceMaterial;

};

SXAVENGER_ENGINE_NAMESPACE_END
