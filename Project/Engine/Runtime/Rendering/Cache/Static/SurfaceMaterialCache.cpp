#include "SurfaceMaterialCache.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cahce
#include "../Manager/CacheCollection.h"

//* engine
#include <Runtime/Graphics/Core.h>
#include <Runtime/Scheduler/Common/TaskHandle.h>
#include <Runtime/Assets/Texture/Texture.h>
#include <Runtime/Assets/Handle/AssetHandle.h>

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Logger/StackLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceMaterialCache class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SurfaceMaterialCache::Cache(const std::shared_ptr<const Assets::SurfaceMaterial>& material) {

	// TODO: TextureCacheの更新に合わせてMaterial側の更新を行う.

	Scheduler::TaskHandle handle = material->GetTaskHandle();

	if (handle.GetState() != Scheduler::TaskState::State::Completed) {
		STACK_LOG_WARNING("Rendering::SurfaceMaterialCache | surface material is not ready. name: {}", material->GetName());
		return; //!< taskが完了していない場合はキャッシュしない
	}

	//!< descriptionの取得
	const Assets::SurfaceMaterial::Description& description = material->GetDescription();

	//!< bufferの作成
	materialBuffer_ = SurfaceMaterialCache::CreateSurfaceMaterialBuffer(material->GetName());

	//!< materialの保持
	material_ = material;

	//!< addressの取得
	address_ = material->GetAddress();

	STREAM_LOG_INFO("Rendering::SurfaceMaterialCache | cache surface material completed. name: {}", material->GetName());
}

void SurfaceMaterialCache::Refresh() {
	if (material_ == nullptr) {
		return; //!< materialがキャッシュされていない場合は何もしない
	}

	if (!materialBuffer_.material.HasHandle()) {
		return; //!< material bufferが作成されていない場合は何もしない
	}

	Scheduler::TaskHandle handle = material_->GetTaskHandle();

	if (handle.GetState() != Scheduler::TaskState::State::Completed) {
		STACK_LOG_WARNING("Rendering::SurfaceMaterialCache | surface material is not ready. name: {}", material_->GetName());
		return; //!< taskが完了していない場合はキャッシュしない
	}

	//!< descriptionの取得
	const Assets::SurfaceMaterial::Description& description = material_->GetDescription();


	//!< textureのキャッシュ
	for (const auto& id : description.textures) {
		if (id.has_value()) {
			Assets::AssetHandle<Assets::Texture> texture = id.value();

			if (texture.GetTaskHandle() == Scheduler::TaskState::State::Completed) {
				CacheCollection::GetInstance()->Cache<TextureCache>(texture.GetUuid(), texture.GetAddress(), texture.Get());
			}
		}
	}

	//!< bufferにmaterialの設定
	SurfaceMaterialBuffer::Description& data = materialBuffer_.material.At();

	//!< albedoの設定
	if (description.HasTexture(Assets::SurfaceMaterial::TextureType::Diffuse)) {

		Uuid uuid = description.GetTextureUuid(Assets::SurfaceMaterial::TextureType::Diffuse);

		if (SurfaceMaterialCache::HasTextureCache(uuid)) {
			//!< cacheからtextureのdescriptorを取得
			std::shared_ptr<const TextureCache> cache = SurfaceMaterialCache::GetTextureCache(uuid);
			const Graphics::Descriptor& descriptor = cache->GetDescriptor();

			data.albedo = Material::Albedo::FromTexture(descriptor.GetHandle());
		}

	} else {
		data.albedo = Material::Albedo::FromColor(description.diffuse);
	}

	//!< transparencyの設定
	if (description.HasTexture(Assets::SurfaceMaterial::TextureType::Diffuse)) {

		Uuid uuid = description.GetTextureUuid(Assets::SurfaceMaterial::TextureType::Diffuse);

		if (SurfaceMaterialCache::HasTextureCache(uuid)) {
			//!< cacheからtextureのdescriptorを取得
			std::shared_ptr<const TextureCache> cache = SurfaceMaterialCache::GetTextureCache(uuid);
			const Graphics::Descriptor& descriptor = cache->GetDescriptor();

			data.transparency = Material::Transparency::FromTexture(descriptor.GetHandle());
		}

	} else {
		data.transparency = Material::Transparency::FromValue(1.0f);
	}

	//!< emissiveの設定
	if (description.HasTexture(Assets::SurfaceMaterial::TextureType::Emissive)) {

		Uuid uuid = description.GetTextureUuid(Assets::SurfaceMaterial::TextureType::Emissive);

		if (SurfaceMaterialCache::HasTextureCache(uuid)) {
			//!< cacheからtextureのdescriptorを取得
			std::shared_ptr<const TextureCache> cache = SurfaceMaterialCache::GetTextureCache(uuid);
			const Graphics::Descriptor& descriptor = cache->GetDescriptor();

			data.emissive = Material::Emissive::FromTexture(descriptor.GetHandle());
		}

	} else {
		data.emissive = Material::Emissive::FromColor(description.emissive);
	}

	//!< normalの設定
	if (description.HasTexture(Assets::SurfaceMaterial::TextureType::Normal)) {

		Uuid uuid = description.GetTextureUuid(Assets::SurfaceMaterial::TextureType::Normal);

		if (SurfaceMaterialCache::HasTextureCache(uuid)) {

			//!< cacheからtextureのdescriptorを取得
			std::shared_ptr<const TextureCache> cache = SurfaceMaterialCache::GetTextureCache(uuid);
			const Graphics::Descriptor& descriptor = cache->GetDescriptor();

			data.normal = Material::Normal::FromTexture(descriptor.GetHandle());
		}

	} else {
		data.normal = Material::Normal::FromValue();
	}

	//!< roughnessの設定
	if (description.HasTexture(Assets::SurfaceMaterial::TextureType::Roughness)) {

		Uuid uuid = description.GetTextureUuid(Assets::SurfaceMaterial::TextureType::Roughness);

		if (SurfaceMaterialCache::HasTextureCache(uuid)) {
			//!< cacheからtextureのdescriptorを取得
			std::shared_ptr<const TextureCache> cache = SurfaceMaterialCache::GetTextureCache(uuid);
			const Graphics::Descriptor& descriptor = cache->GetDescriptor();

			data.roughness = Material::Roughness::FromTexture(descriptor.GetHandle());
		}

	} else {
		data.roughness = Material::Roughness::FromValue(description.roughness);
	}

	//!< metallicの設定
	if (description.HasTexture(Assets::SurfaceMaterial::TextureType::Metallic)) {

		Uuid uuid = description.GetTextureUuid(Assets::SurfaceMaterial::TextureType::Metallic);

		if (SurfaceMaterialCache::HasTextureCache(uuid)) {
			//!< cacheからtextureのdescriptorを取得
			std::shared_ptr<const TextureCache> cache = SurfaceMaterialCache::GetTextureCache(uuid);
			const Graphics::Descriptor& descriptor = cache->GetDescriptor();

			data.metallic = Material::Metallic::FromTexture(descriptor.GetHandle());
		}

	} else {
		data.metallic = Material::Metallic::FromValue(description.metallic);
	}
}

bool SurfaceMaterialCache::HasTextureCache(const Uuid& uuid) {
	return CacheCollection::GetInstance()->HasCache(uuid);
}

std::shared_ptr<const TextureCache> SurfaceMaterialCache::GetTextureCache(const Uuid& uuid) {
	STREAM_ASSERT(CacheCollection::GetInstance()->HasCache(uuid), "texture cache is not found. uuid: {}", uuid.Serialize());
	return CacheCollection::GetInstance()->GetCache<const TextureCache>(uuid);
}

SurfaceMaterialBuffer SurfaceMaterialCache::CreateSurfaceMaterialBuffer(const std::string_view& name) {

	//!< bufferの作成
	SurfaceMaterialBuffer buffer = SurfaceMaterialBuffer::Create(
		Graphics::kFrameCount
	);
	buffer.material.SetName(std::format("SurfaceMaterialCache | SurfaceMaterialBuffer | {}", name));

	// TODO: Assets::SurfaceMaterial側にtextureとvalueの演算方法を設定する変数を追加する.
	// todo: Transform2dの設定を追加する場所の検討.
	
	return buffer;
	
}
