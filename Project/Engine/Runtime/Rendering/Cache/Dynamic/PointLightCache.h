#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cahce
#include "BaseDynamicCache.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/ConstantBuffer.h>
#include <Runtime/World/Component/Light/Punctual/PointLightComponent.h>

//* lib
#include <Lib/Math/Matrix4x4.h>
#include <Lib/Math/Transform/Transformation3d.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// PointLightCache class
////////////////////////////////////////////////////////////////////////////////////////////
class PointLightCache final
	: public BaseDynamicCache {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// LightData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct LightData {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		static LightData Transfer(
			const World::PointLightComponent::LightParameter& light,
			const World::PointLightComponent::ShadowParameter& shadow
		);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Color3f lightColor;
		float lightIntensity; //!< [cd/m^2]
		float lightRadius;

		FlagEnum<World::ShadowFlags> shadowFlag;
		float shadowStrength;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	PointLightCache() noexcept  = default;
	~PointLightCache() override = default;

	//* cache option *//

	void Cache(const World::PointLightComponent& light);

	const Graphics::ConstantBuffer<LightData>& GetLightBuffer() const { return lightBuffer_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* projection *//

	Graphics::ConstantBuffer<LightData> lightBuffer_ = {};

};

SXAVENGER_ENGINE_NAMESPACE_END
