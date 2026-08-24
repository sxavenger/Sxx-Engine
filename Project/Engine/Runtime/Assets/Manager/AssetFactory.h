#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/BaseAssetMetadata.h"
#include "../Base/BaseAsset.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/EnumUtil.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssetFactory class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetFactory final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	template <Asset T>
	static std::shared_ptr<T> Create(const std::filesystem::path& filepath);

	static std::shared_ptr<BaseAsset> Create(const std::filesystem::path& filepath, AssetType type);
	//!< TODO: AssetTypeに応じてCreateするAssetを切り替える

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	static BaseAssetMetadata LoadMetadata(const std::filesystem::path& filepath);

};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetFactory class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Asset T>
std::shared_ptr<T> AssetFactory::Create(const std::filesystem::path& filepath) {
	BaseAssetMetadata metadata = AssetFactory::LoadMetadata(filepath); //!< metadataの取得
	STREAM_ASSERT(
		metadata.type == T::GetStaticType(),
		"asset type mismatch. expected: {}, actual: {}", T::GetStaticType(), metadata.type
	); //!< Assetの型が一致しない場合は例外を投げる

	return std::make_shared<T>(metadata); //!< Assetの生成
}

SXAVENGER_ENGINE_NAMESPACE_END
