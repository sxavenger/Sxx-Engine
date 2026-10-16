#include "StaticMeshImporter.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Assimp/AssimpCommon.h"
#include "../Assimp/AssimpImporter.h"
#include "../Base/BaseAssetMetadata.h"
#include "../Mesh/StaticMesh.h"
#include "../Mesh/StaticMeshTraits.h"
#include "../Manager/AssetStorage.h"

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonFile.h>
#include <Lib/Format/Json/JsonNode.h>

////////////////////////////////////////////////////////////////////////////////////////////
// StaticMeshImporter class methods
////////////////////////////////////////////////////////////////////////////////////////////

Uuid StaticMeshImporter::Import(const std::filesystem::path& filepath, const ReferenceParameters& parameters) {
	STREAM_ASSERT(std::filesystem::exists(filepath), "static mesh file does not exist. filepath: {}", filepath.generic_string()); //!< ファイルが存在するか確認

	FileType type = Assets::CheckFileType(filepath.extension()); //!< ファイルの種類を判定

	std::filesystem::path parent   = filepath.parent_path(); //!< 親ディレクトリを取得
	std::filesystem::path filename = filepath.filename(); //!< ファイル名を取得

	if (type == FileType::Other) {
		//!< Assetファイル以外の場合はMetadataを作成.

		//! directoryの追加
		parent /= StaticMesh::kDirectory;
		std::filesystem::create_directories(parent); //!< ディレクトリが存在しない場合は作成する

		//!< metadataのファイル名の作成
		const AssimpMesh& mesh = AssimpCommon::GetImporter(filepath).GetMesh(parameters.index); //!< assimpのmeshを取得
		filename = std::filesystem::path(mesh.GetName()).replace_extension(kAssetFileExtension); //!< ファイル名をmesh名に変更

		//!< metadataが存在しているか確認する.
		if (std::filesystem::exists(parent / filename)) {
			return AssetStorage::GetInstance()->Import<StaticMesh>(parent / filename); //!< AssetStorageに登録
		}

		json::node node = json::node::object();

		{ //!< metadataの作成
			
			BaseAssetMetadata metadata = {};
			metadata.uuid = Uuid::Generate(); //!< 新しいuuidを生成
			metadata.type = AssetTraits<StaticMesh>::kType;

			node = BaseAssetMetadata::Serialize(metadata); //!< metadataをjsonに変換
		}

		{ //!< static meshのmetadataの作成

			StaticMeshMetadata::ReferenceData reference = {};
			reference.filepath = filepath.lexically_relative(parent); //!< 親ディレクトリからの相対パスを保持する
			reference.index    = parameters.index;

			//!< metadataの作成
			StaticMeshMetadata metadata = {};
			metadata.data = reference;

			node["metadata"] = StaticMeshMetadata::Serialize(metadata); //!< metadataをjsonに変換
		}

		JsonFile::Write(parent / filename, node);
		STREAM_LOG_DEBUG("Assets::StaticMeshImporter | static mesh metadata created. filepath: {}", (parent / filename).generic_string()); //!< metadataを作成したことを通知
	}

	return AssetStorage::GetInstance()->Import<StaticMesh>(parent / filename); //!< AssetStorageに登録

}
