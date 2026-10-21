#include "DirectionalLightCache.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [DirectionalLightCache] DirectionalLightData structure
////////////////////////////////////////////////////////////////////////////////////////////

DirectionalLightCache::DirectionalLightData DirectionalLightCache::DirectionalLightData::Transfer(
	const World::DirectionalLightComponent::LightParameter& light,
	const World::DirectionalLightComponent::ShadowParameter& shadow) {

	DirectionalLightData data = {};
	data.lightColor     = light.color;
	data.lightIntensity = std::max(0.0f, light.intensity);
	data.shadowFlag     = shadow.flag;
	data.shadowStrength = std::clamp(shadow.strength, 0.0f, 1.0f);
	
	return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
// DirectionalLightCache class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectionalLightCache::Cache(const World::DirectionalLightComponent& light) {

	if (!directionalLightBuffer_.HasHandle()) { //!< directionalLightBufferが作成されていない場合
		//!< directionalLightの作成
		directionalLightBuffer_ = Graphics::Core::CreateConstantBuffer<DirectionalLightData>(
			Graphics::kFrameCount
		);

		directionalLightBuffer_.SetName(L"DirectionalLightCache");
	}

	//!< directionalLightの転送
	directionalLightBuffer_.At() = DirectionalLightData::Transfer(light.GetLightParameter(), light.GetShadowParameter());

}
