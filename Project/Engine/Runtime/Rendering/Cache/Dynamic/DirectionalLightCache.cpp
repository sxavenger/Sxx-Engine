#include "DirectionalLightCache.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [DirectionalLightCache] LightData structure
////////////////////////////////////////////////////////////////////////////////////////////

DirectionalLightCache::LightData DirectionalLightCache::LightData::Transfer(
	const World::DirectionalLightComponent::LightParameter& light,
	const World::DirectionalLightComponent::ShadowParameter& shadow) {

	LightData data = {};
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

	if (!lightBuffer_.HasHandle()) { //!< lightBufferが作成されていない場合
		//!< lightの作成
		lightBuffer_ = Graphics::Core::CreateConstantBuffer<LightData>(
			Graphics::kFrameCount
		);

		lightBuffer_.SetName(L"DirectionalLightCache | LightData");
	}

	//!< directionalLightの転送
	lightBuffer_.At() = LightData::Transfer(light.GetLightParameter(), light.GetShadowParameter());

}
