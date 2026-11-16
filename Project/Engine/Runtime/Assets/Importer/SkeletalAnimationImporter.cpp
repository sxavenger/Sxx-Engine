#include "SkeletalAnimationImporter.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Assimp/AssimpCommon.h"
#include "../Assimp/AssimpImporter.h"
#include "../Base/BaseAssetMetadata.h"
#include "../Animation/SkeletalAnimationMetadata.h"
#include "../Animation/SkeletalAnimationTraits.h"
#include "../Manager/AssetStorage.h"

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonFile.h>
#include <Lib/Format/Json/JsonNode.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletalAnimationImporter class methods
////////////////////////////////////////////////////////////////////////////////////////////

SkeletalAnimationContents SkeletalAnimationImporter::Import(const std::filesystem::path& filepath, const ReferenceParameters& parameters) {
	STREAM_ASSERT(std::filesystem::exists(filepath), "skeletal animation file does not exist. filepath: {}", filepath.generic_string()); //!< ファイルが存在するか確認

	SkeletalAnimationContents contents = {};

	FileType type = Assets::CheckFileType(filepath.extension()); //!< ファイルの種類を判定

	std::filesystem::path parent   = filepath.parent_path(); //!< 親ディレクトリを取得
	std::filesystem::path filename = filepath.filename(); //!< ファイル名を取得

	if (type == FileType::Other) {
		//!< Assetファイル以外の場合はMetadataを作成.

		//! directoryの追加
		parent /= SkeletalAnimation::kDirectory;
		std::filesystem::create_directories(parent); //!< ディレクトリが存在しない場合は作成する

		//!< metadataのファイル名の作成
		const AssimpAnimation& animation = AssimpCommon::GetImporter(filepath).GetAnimation(parameters.index); //!< assimpのanimationを取得

		std::string name = std::string(animation.GetName());
		if (name.empty()) {
			name = std::format("SkeletalAnimation_{}", parameters.index); //!< animation名が空の場合は, "SkeletalAnimation_{index}"という名前を付ける
		}

		filename = std::filesystem::path(name).replace_extension(kAssetFileExtension); //!< ファイル名をanimation名に変更

		//!< metadataが存在しているか確認する.
		if (std::filesystem::exists(parent / filename)) {
			contents.id = AssetStorage::GetInstance()->Import<SkeletalAnimation>(parent / filename); //!< AssetStorageに登録
			return contents;
		}

		json::node node = json::node::object();

		{ //!< metadataの作成
			
			BaseAssetMetadata metadata = {};
			metadata.uuid = Uuid::Generate(); //!< 新しいuuidを生成
			metadata.type = AssetTraits<SkeletalAnimation>::kType;

			node = BaseAssetMetadata::Serialize(metadata); //!< metadataをjsonに変換
		}

		{ //!< skeletal animationのmetadataの作成

			SkeletalAnimationMetadata::ReferenceData reference = {};
			reference.filepath = filepath.lexically_relative(parent); //!< 親ディレクトリからの相対パスを保持する
			reference.index    = parameters.index;

			//!< metadataの作成
			SkeletalAnimationMetadata metadata = {};
			metadata.data = reference;

			node["metadata"] = SkeletalAnimationMetadata::Serialize(metadata); //!< metadataをjsonに変換
		}

		JsonFile::Write(parent / filename, node);
		STREAM_LOG_DEBUG("Assets::SkeletalAnimationImporter | skeletal animation metadata created. filepath: {}", (parent / filename).generic_string()); //!< metadataを作成したことを通知
	}

	contents.id = AssetStorage::GetInstance()->Import<SkeletalAnimation>(parent / filename); //!< AssetStorageに登録
	return contents;
}
