#include "AssetFactory.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonFile.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetFactory class methods
////////////////////////////////////////////////////////////////////////////////////////////

BaseAssetMetadata AssetFactory::LoadMetadata(const std::filesystem::path& filepath) {
	STREAM_ASSERT(
		std::filesystem::exists(filepath),
		"asset file does not exist. filepath: {}", filepath.generic_string()
	); //!< ファイルが存在しない場合は例外を投げる

	//!< Assetファイルの読み込み
	json::node node = JsonFile::Load(filepath);

	return BaseAssetMetadata::Deserialize(filepath, node); //!< metadataの取得
}
