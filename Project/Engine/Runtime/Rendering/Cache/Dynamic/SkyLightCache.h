#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cahce
#include "BaseDynamicCache.h"
#include "../Static/TextureCache.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/ConstantBuffer.h>
#include <Runtime/World/Component/Light/Environment/SkyLightComponent.h>

//* lib
#include <Lib/Math/Matrix4x4.h>
#include <Lib/Math/Transform/Transformation3d.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightCache class
////////////////////////////////////////////////////////////////////////////////////////////
class SkyLightCache final
	: public BaseDynamicCache {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// LightData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct LightData {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Type enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Type : uint32_t {
			None            = 0,
			Equirectangular = 1,
			Cube            = 2,
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		static LightData Transfer(
			const World::SkyLightComponent::LightParameter& light,
			const Assets::AssetHandle<Assets::Texture>& environmentTexture
		);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Color3f lightColor;
		float lightIntensity; //!< [cd]
		
		uint32_t environmentTextureIndex;
		Type environmentTextureType;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	SkyLightCache() noexcept  = default;
	~SkyLightCache() override = default;

	//* cache option *//

	void Cache(const World::SkyLightComponent& light);

	const Graphics::ConstantBuffer<LightData>& GetLightBuffer() const { return lightBuffer_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* projection *//

	Graphics::ConstantBuffer<LightData> lightBuffer_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	static bool HasTextureCache(const Uuid& uuid);

	static std::shared_ptr<const TextureCache> GetTextureCache(const Uuid& uuid);

};

SXAVENGER_ENGINE_NAMESPACE_END
