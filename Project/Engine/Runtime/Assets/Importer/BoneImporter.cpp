#include "BoneImporter.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Assimp/AssimpCommon.h"
#include "../Assimp/AssimpImporter.h"
#include "../Base/BaseAssetMetadata.h"
#include "../Bone/Bone.h"
#include "../Bone/BoneTraits.h"
#include "../Manager/AssetStorage.h"

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonFile.h>
#include <Lib/Format/Json/JsonNode.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BoneImporter class methods
////////////////////////////////////////////////////////////////////////////////////////////

BoneContents BoneImporter::Import(const std::filesystem::path& filepath, const ReferenceParameters& parameters) {
	STREAM_ASSERT(std::filesystem::exists(filepath), "bone file does not exist. filepath: {}", filepath.generic_string()); //!< ファイルが存在するか確認

	BoneContents contents = {};

	FileType type = Assets::CheckFileType(filepath.extension()); //!< ファイルの種類を判定

	std::filesystem::path parent   = filepath.parent_path(); //!< 親ディレクトリを取得
	std::filesystem::path filename = filepath.filename(); //!< ファイル名を取得

	if (type == FileType::Other) {
		//!< Assetファイル以外の場合はMetadataを作成.

		//! directoryの追加
		parent /= Bone::kDirectory;
		std::filesystem::create_directories(parent); //!< ディレクトリが存在しない場合は作成する

		//!< metadataのファイル名の作成
		filename.replace_extension(kAssetFileExtension); //!< 拡張子をmetadata指定に変更

		//!< metadataが存在しているか確認する.
		if (std::filesystem::exists(parent / filename)) {
			contents.id = AssetStorage::GetInstance()->Import<Bone>(parent / filename); //!< AssetStorageに登録
			return contents;
		}

		json::node node = json::node::object();

		{ //!< metadataの作成

			BaseAssetMetadata metadata = {};
			metadata.uuid = Uuid::Generate(); //!< 新しいuuidを生成
			metadata.type = AssetTraits<Bone>::kType;

			node = BaseAssetMetadata::Serialize(metadata); //!< metadataをjsonに変換
		}

		{ //!< boneのmetadataの作成

			BoneMetadata::ReferenceData reference = {};
			reference.filepath = filepath.lexically_relative(parent); //!< 親ディレクトリからの相対パスを保持する

			//!< metadataの作成
			BoneMetadata metadata = {};
			metadata.data = reference;

			node["metadata"] = BoneMetadata::Serialize(metadata); //!< metadataをjsonに変換
		}

		JsonFile::Write(parent / filename, node);
		STREAM_LOG_DEBUG("Assets::BoneImporter | bone metadata created. filepath: {}", (parent / filename).generic_string()); //!< metadataを作成したことを通知
	}

	contents.id = AssetStorage::GetInstance()->Import<Bone>(parent / filename); //!< AssetStorageに登録
	return contents;

}
