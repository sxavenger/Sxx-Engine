#include "SkyLightCache.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cahce
#include "../Manager/CacheCollection.h"

//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [SkyLightCache] LightData structure
////////////////////////////////////////////////////////////////////////////////////////////

SkyLightCache::LightData SkyLightCache::LightData::Transfer(
	const World::SkyLightComponent::LightParameter& light,
	const Assets::AssetHandle<Assets::Texture>& environmentTexture) {

	LightData data = {};
	data.lightColor     = light.color;
	data.lightIntensity = std::max(0.0f, light.GetIntensity(World::LightingScale::Physical)); //!< [lux]に変換して格納

	if (environmentTexture.HasUuid() && !SkyLightCache::HasTextureCache(environmentTexture.GetUuid())) {
		if (environmentTexture.GetTaskHandle() == Scheduler::TaskState::State::Completed) {
			//!< textureのキャッシュが存在しない場合はキャッシュを作成する
			std::shared_ptr<const Assets::Texture> texture = environmentTexture.WaitGet();
			CacheCollection::GetInstance()->Cache<TextureCache>(texture->GetUuid(), texture->GetAddress(), texture);
		}
	}

	if (environmentTexture.HasUuid() && SkyLightCache::HasTextureCache(environmentTexture.GetUuid())) {

		std::shared_ptr<const Assets::Texture> texture   = environmentTexture.WaitGet();
		std::shared_ptr<const TextureCache> textureCache = SkyLightCache::GetTextureCache(environmentTexture.GetUuid());

		data.environmentTextureIndex = textureCache->GetDescriptor().GetIndex();
		data.environmentTextureType  = (texture->GetDescription().IsCubemap()) ? Type::Cube : Type::Equirectangular; //!< cubemapかequirectangularかを判定して格納

	} else {
		data.environmentTextureIndex = NULL;
		data.environmentTextureType  = Type::None;
	}
	
	return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightCache class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyLightCache::Cache(const World::SkyLightComponent& light) {

	if (!lightBuffer_.HasHandle()) { //!< lightBufferが作成されていない場合
		//!< lightの作成
		lightBuffer_ = Graphics::Core::CreateConstantBuffer<LightData>(
			Graphics::kFrameCount
		);

		lightBuffer_.SetName(L"PointLightCache | LightData");
	}

	//!< directionalLightの転送
	lightBuffer_.At() = LightData::Transfer(light.GetLightParameter(), light.GetEnvironment());

}

bool SkyLightCache::HasTextureCache(const Uuid& uuid) {
	return CacheCollection::GetInstance()->HasCache(uuid);
}

std::shared_ptr<const TextureCache> SkyLightCache::GetTextureCache(const Uuid& uuid) {
	STREAM_ASSERT(CacheCollection::GetInstance()->HasCache(uuid), "texture cache is not found. uuid: {}", uuid.Serialize());
	return CacheCollection::GetInstance()->GetCache<const TextureCache>(uuid);
}
