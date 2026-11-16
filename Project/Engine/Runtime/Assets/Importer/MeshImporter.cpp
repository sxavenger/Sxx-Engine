#include "MeshImporter.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Assimp/AssimpCommon.h"
#include "../Assimp/AssimpImporter.h"
#include "StaticMeshImporter.h"
#include "SkeletalMeshImporter.h"

//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletalMeshImporter class methods
////////////////////////////////////////////////////////////////////////////////////////////

MeshContents MeshImporter::Import(const std::filesystem::path& filepath, const ReferenceParameters& parameters) {
	STREAM_ASSERT(std::filesystem::exists(filepath), "file does not exist. filepath: {}", filepath.generic_string()); //!< ファイルが存在するか確認
	STREAM_ASSERT(Assets::CheckFileType(filepath.extension()) == FileType::Other, "file is not a supported type. filepath: {}", filepath.generic_string()); //!< Assetファイルの場合は, MeshImporterではなく各Importerを使用する

	MeshContents contents = {};

	FileType type = Assets::CheckFileType(filepath.extension()); //!< ファイルの種類を判定

	std::filesystem::path parent   = filepath.parent_path(); //!< 親ディレクトリを取得
	std::filesystem::path filename = filepath.filename(); //!< ファイル名を取得

	if (type == FileType::Other) {
		//!< importerの取得
		const AssimpImporter& importer = AssimpCommon::GetImporter(filepath);
		const AssimpMesh mesh = importer.GetMesh(parameters.index);

		if (mesh.HasBone()) { //!< ボーンを持つメッシュの場合はSkeletalMeshImporterを使用
			contents.id   = SkeletalMeshImporter::Import(filepath, SkeletalMeshImporter::ReferenceParameters{ parameters.index }).id;
			contents.type = MeshContents::Type::Skeletal;

		} else { //!< ボーンを持たないメッシュの場合はStaticMeshImporterを使用
			contents.id   = StaticMeshImporter::Import(filepath, StaticMeshImporter::ReferenceParameters{ parameters.index }).id;
			contents.type = MeshContents::Type::Static;
		}
	}

	return contents;
}
