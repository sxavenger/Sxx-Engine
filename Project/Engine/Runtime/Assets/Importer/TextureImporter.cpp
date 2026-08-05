#include "TextureImporter.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/BaseAssetMetadata.h"
#include "../Texture/TextureMetadata.h"
#include "../Manager/AssetStorage.h"

//* lib
#include <Lib/Format/Json/JsonFile.h>
#include <Lib/Format/Json/JsonNode.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TextureImporter class methods
////////////////////////////////////////////////////////////////////////////////////////////

Uuid TextureImporter::Import(const std::filesystem::path& filepath) {

	FileType type = Assets::CheckFileType(filepath.extension()); //!< ファイルの種類を判定

	std::filesystem::path parent   = filepath.parent_path(); //!< 親ディレクトリを取得
	std::filesystem::path filename = filepath.filename(); //!< ファイル名を取得

	if (type == FileType::Other) {
		//!< Assetファイル以外の場合はMetadataを作成.

		//!< metadataのファイル名の作成
		filename.replace_extension(kAssetFileExtension); //!< 拡張子をmetadata指定に変更

		// TODO: metadataが存在しているか確認する.

		json::node node = json::node::object();

		{
			//!< metadataの保存
			BaseAssetMetadata metadata = {};
			metadata.uuid = Uuid::Generate(); //!< 新しいuuidを生成
			metadata.type = AssetType::Texture;

			node = BaseAssetMetadata::Serialize(metadata); //!< metadataをjsonに変換
		}

		{ //!< textureのmetadataの作成
			//!< filepathを参照するデータの作成
			TextureMetadata::ReferenceData reference = {};
			reference.filepath = filepath.filename(); //!< filenameのみを保持する

			//!< metadataの作成
			TextureMetadata metadata = {};
			metadata.data = reference;

			node["metadata"] = TextureMetadata::Serialize(metadata); //!< metadataをjsonに変換
		}
		
		JsonFile::Write(parent / filename, node);
		StreamLogger::Debug("Assets::TextureImporter | texture metadata created. filepath: {}", (parent / filename).generic_string()); //!< metadataを作成したことを通知
	}

	return AssetStorage::GetInstance()->Import<Texture>(parent / filename); //!< AssetStorageに登録
}
