#include "TextureImporter.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/BaseAssetMetadata.h"
#include "../Texture/TextureMetadata.h"
#include "../Texture/TextureTraits.h"
#include "../Manager/AssetStorage.h"

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonFile.h>
#include <Lib/Format/Json/JsonNode.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TextureImporter class methods
////////////////////////////////////////////////////////////////////////////////////////////

Uuid TextureImporter::Import(const std::filesystem::path& filepath, const ReferenceParameters& parameters) {
	STREAM_ASSERT(std::filesystem::exists(filepath), "Assets::TextureImporter | file does not exist. filepath: {}", filepath.generic_string()); //!< ファイルが存在するか確認

	FileType type = Assets::CheckFileType(filepath.extension()); //!< ファイルの種類を判定

	std::filesystem::path parent   = filepath.parent_path(); //!< 親ディレクトリを取得
	std::filesystem::path filename = filepath.filename(); //!< ファイル名を取得

	if (type == FileType::Other) {
		//!< Assetファイル以外の場合はMetadataを作成.

		//!< metadataのファイル名の作成
		filename.replace_extension(kAssetFileExtension); //!< 拡張子をmetadata指定に変更

		//!< metadataが存在しているか確認する.
		if (std::filesystem::exists(parent / filename)) {
			return AssetStorage::GetInstance()->Import<Texture>(parent / filename); //!< AssetStorageに登録
		}

		json::node node = json::node::object();

		{ //!< metadataの作成
			
			BaseAssetMetadata metadata = {};
			metadata.uuid = Uuid::Generate(); //!< 新しいuuidを生成
			metadata.type = AssetTraits<Texture>::kType;

			node = BaseAssetMetadata::Serialize(metadata); //!< metadataをjsonに変換
		}

		{ //!< textureのmetadataの作成
			
			TextureMetadata::ReferenceData reference = {};
			reference.filepath          = filepath.lexically_relative(parent); //!< 親ディレクトリからの相対パスを保持する
			reference.encoding          = parameters.encoding;
			reference.isGenerateMipmaps = parameters.isGenerateMipmap;

			//!< metadataの作成
			TextureMetadata metadata = {};
			metadata.data = reference;

			node["metadata"] = TextureMetadata::Serialize(metadata); //!< metadataをjsonに変換
		}
		
		JsonFile::Write(parent / filename, node);
		STREAM_LOG_DEBUG("Assets::TextureImporter | texture metadata created. filepath: {}", (parent / filename).generic_string()); //!< metadataを作成したことを通知
	}

	return AssetStorage::GetInstance()->Import<Texture>(parent / filename); //!< AssetStorageに登録
	
}
