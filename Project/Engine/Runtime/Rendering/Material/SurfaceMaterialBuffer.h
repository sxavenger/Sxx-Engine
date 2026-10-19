#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* material
#include "Albedo.h"
#include "Transparency.h"
#include "Emissive.h"
#include "Normal.h"
#include "Roughness.h"
#include "Metallic.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/ConstantBuffer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceMaterialBuffer structure
////////////////////////////////////////////////////////////////////////////////////////////
struct SurfaceMaterialBuffer final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Description structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Description final {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Material::Albedo albedo;
		Material::Transparency transparency;
		Material::Emissive emissive;
		Material::Normal normal;
		Material::Roughness roughness;
		Material::Metallic metallic;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static SurfaceMaterialBuffer Create(uint8_t frameCount);

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* Buffer *//

	Graphics::ConstantBuffer<Description> material;

};

SXAVENGER_ENGINE_NAMESPACE_END
