#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cahce
#include "BaseDynamicCache.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/ConstantBuffer.h>
#include <Runtime/World/Component/Light/Punctual/DirectionalLightComponent.h>

//* lib
#include <Lib/Math/Matrix4x4.h>
#include <Lib/Math/Transform/Transformation3d.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// DirectionalLightCache class
////////////////////////////////////////////////////////////////////////////////////////////
class DirectionalLightCache final
	: public BaseDynamicCache {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// DirectionalLightData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct DirectionalLightData {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		static DirectionalLightData Transfer(
			const World::DirectionalLightComponent::LightParameter& light,
			const World::DirectionalLightComponent::ShadowParameter& shadow
		);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Color3f lightColor;
		float lightIntensity;

		FlagEnum<World::ShadowFlags> shadowFlag;
		float shadowStrength;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	DirectionalLightCache() noexcept  = default;
	~DirectionalLightCache() override = default;

	//* cache option *//

	void Cache(const World::DirectionalLightComponent& light);

	const Graphics::ConstantBuffer<DirectionalLightData>& GetDirectionalLightBuffer() const { return directionalLightBuffer_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* projection *//

	Graphics::ConstantBuffer<DirectionalLightData> directionalLightBuffer_ = {};

};

SXAVENGER_ENGINE_NAMESPACE_END
