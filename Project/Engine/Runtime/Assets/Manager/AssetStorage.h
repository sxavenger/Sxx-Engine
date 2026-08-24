#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/BaseAsset.h"
#include "../Texture/TextureTraits.h"
#include "../Mesh/StaticMeshTraits.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Scheduler/Cpu/CpuTask.h>

//* lib
#include <Lib/Uuid/Uuid.h>

//* c++
#include <unordered_map>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssetStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetStorage final {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Storage = std::unordered_map<Uuid, std::shared_ptr<BaseAsset>>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* asset option *//

	template <Asset T>
	Uuid Import(const std::filesystem::path& filepath);

	template <Asset T>
	std::shared_ptr<T> Get(const Uuid& id) const;

	template <Asset T>
	void Reload(const Uuid& id);

	//* singleton *//

	static AssetStorage* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Storage storage_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* metadata option *//

	static BaseAssetMetadata LoadMetadata(const std::filesystem::path& filepath);

	//* cast option *//

	template <Asset T>
	static std::shared_ptr<T> Cast(const std::shared_ptr<BaseAsset>& asset);

	//* task option *//

	static void PushBuildTask(const std::shared_ptr<BaseAsset>& asset, const Scheduler::CpuTask::Function& function);

};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetStorage class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Asset T>
Uuid AssetStorage::Import(const std::filesystem::path& filepath) {
	
	BaseAssetMetadata metadata = AssetStorage::LoadMetadata(filepath); //!< metadataの取得

	if (storage_.contains(metadata.uuid)) {
		STREAM_LOG_WARNING("Assets::AssetStorage | asset already imported. filepath: {}, uuid: {}", filepath.generic_string(), metadata.uuid.Serialize());
		return metadata.uuid; //!< すでに登録されている場合は警告を出す
	}

	//!< Assetの登録
	std::shared_ptr<T> asset = std::make_shared<T>(metadata);
	storage_[metadata.uuid]  = asset; 

	//!< Assetのビルドを非同期実行.
	AssetStorage::PushBuildTask(asset, [asset]() mutable { AssetTraits<T>::Build(asset); });

	return metadata.uuid;
}

template <Asset T>
std::shared_ptr<T> AssetStorage::Get(const Uuid& id) const {
	STREAM_ASSERT(storage_.contains(id), "asset not found. uuid: {}", id.Serialize()); //!< 存在しない場合は例外を投げる
	return AssetStorage::Cast<T>(storage_.at(id));
}

template <Asset T>
std::shared_ptr<T> AssetStorage::Cast(const std::shared_ptr<BaseAsset>& asset) {
	std::shared_ptr<T> pointer = std::static_pointer_cast<T>(asset);
	STREAM_ASSERT(pointer != nullptr, "failed to cast asset.");
	return pointer;
}

template <Asset T>
void AssetStorage::Reload(const Uuid& id) {
	if (!storage_.contains(id)) {
		STREAM_LOG_WARNING("Assets::AssetStorage | asset not found. uuid: {}", id.Serialize());
		return; //!< 存在しない場合は何もしない
	}

	std::shared_ptr<BaseAsset> asset = storage_.at(id); //!< 旧のAssetの取得
	BaseAssetMetadata metadata       = AssetStorage::LoadMetadata(asset->GetFilepath()); //!< metadataの取得

	if (metadata.uuid != id) {
		STREAM_LOG_WARNING("Assets::AssetStorage | asset uuid is mismatched. filepath: {}, old uuid: {}, new uuid: {}", asset->GetFilepath().generic_string(), id.Serialize(), metadata.uuid.Serialize());
		return;
	}

	std::shared_ptr<T> newAsset = std::make_shared<T>(metadata); //!< 新しいAssetの生成
	storage_[metadata.uuid]     = newAsset; //!< 新しいAssetの登録

	AssetStorage::PushBuildTask(newAsset, [newAsset]() mutable { AssetTraits<T>::Build(newAsset); }); //!< 新しいAssetのビルドを非同期実行.
}

SXAVENGER_ENGINE_NAMESPACE_END
