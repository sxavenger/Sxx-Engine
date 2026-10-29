#include "PointLightCache.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [PointLightCache] LightData structure
////////////////////////////////////////////////////////////////////////////////////////////

PointLightCache::LightData PointLightCache::LightData::Transfer(
	const World::PointLightComponent::LightParameter& light,
	const World::PointLightComponent::ShadowParameter& shadow) {

	LightData data = {};
	data.lightColor     = light.color;
	data.lightIntensity = std::max(0.0f, light.GetIntensity(World::LightUnit::Candela)); //!< [cd/m^2]に変換して格納
	data.lightRadius    = std::max(0.0f, light.radius);
	data.shadowFlag     = shadow.flag;
	data.shadowStrength = std::clamp(shadow.strength, 0.0f, 1.0f);

	return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
// PointLightCache class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PointLightCache::Cache(const World::PointLightComponent& light) {

	if (!lightBuffer_.HasHandle()) { //!< lightBufferが作成されていない場合
		//!< lightの作成
		lightBuffer_ = Graphics::Core::CreateConstantBuffer<LightData>(
			Graphics::kFrameCount
		);

		lightBuffer_.SetName(L"PointLightCache | LightData");
	}

	//!< directionalLightの転送
	lightBuffer_.At() = LightData::Transfer(light.GetLightParameter(), light.GetShadowParameter());

}
