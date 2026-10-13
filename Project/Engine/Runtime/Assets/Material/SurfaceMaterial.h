#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/BaseAsset.h"
#include "SurfaceMaterialMetadata.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Color3.h>
#include <Lib/Math/Transform/Transform2d.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceMaterial class
////////////////////////////////////////////////////////////////////////////////////////////
class SurfaceMaterial final
	: public BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Description structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Description {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================



		//=========================================================================================
		// public variables
		//=========================================================================================

		std::optional<Uuid> diffuseTextureId;
		std::optional<Uuid> emissiveTextureId;
		std::optional<Uuid> normalTextureId;
		std::optional<Uuid> roughnessTextureId;
		std::optional<Uuid> metallicTextureId;

		Transform2d transform;

		Color3f diffuse;
		Color3f emissive;
		float roughness;
		float metallic;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	SurfaceMaterial(const BaseAssetMetadata& metadata) noexcept : BaseAsset(metadata) {}
	~SurfaceMaterial() noexcept override = default;

	//* description option *//

	const Description& GetDescription() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* metadata *//

	SurfaceMaterialMetadata metadata_;

	//* description *//

	Description description_;

	//-----------------------------------------------------------------------------------------
	// friend class
	//-----------------------------------------------------------------------------------------
	friend class SurfaceMaterialBuilder;

};

SXAVENGER_ENGINE_NAMESPACE_END
