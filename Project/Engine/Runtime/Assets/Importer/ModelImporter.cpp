#include "ModelImporter.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Assimp/AssimpCommon.h"
#include "../Assimp/AssimpImporter.h"
#include "StaticMeshImporter.h"
#include "SkeletalMeshImporter.h"
#include "MeshImporter.h"
#include "SurfaceMaterialImporter.h"
#include "BoneImporter.h"
#include "AnimationSequenceImporter.h"

//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelImporter class methods
////////////////////////////////////////////////////////////////////////////////////////////

ModelContents ModelImporter::Import(const std::filesystem::path& filepath) {
	STREAM_ASSERT(std::filesystem::exists(filepath), "file does not exist. filepath: {}", filepath.generic_string()); //!< ファイルが存在するか確認
	STREAM_ASSERT(Assets::CheckFileType(filepath.extension()) == FileType::Other, "file is not a valid model file. filepath: {}", filepath.generic_string());

	ModelContents contents = {};

	//!< importerの取得
	const AssimpImporter& importer = AssimpCommon::GetImporter(filepath);

	//!< materialの取り込み
	for (uint32_t i = 0; i < importer.GetMaterialCount(); ++i) {
		SurfaceMaterialImporter::ReferenceParameters parameters = {};
		parameters.index = i;

		contents.materialIds.emplace_back(SurfaceMaterialImporter::Import(filepath, parameters).id);
	}

	//!< meshの取り込み
	for (uint32_t i = 0; i < importer.GetMeshCount(); ++i) {

		const AssimpMesh mesh = importer.GetMesh(i);

		MeshContents meshContents = MeshImporter::Import(filepath, MeshImporter::ReferenceParameters{ .index = i });

		switch (meshContents.type) {
			case MeshContents::Type::Static:
				contents.staticMeshIds.emplace_back(meshContents.id);
				break;

			case MeshContents::Type::Skeletal:
				contents.skeletalMeshIds.emplace_back(meshContents.id);
				break;
		}

		//!< meshに対応するmaterialのuuidを格納する
		contents.meshMaterialMap[meshContents.id] = contents.materialIds[mesh.GetMaterialIndex()];
	}

	//!< boneの取り込み
	contents.boneId = BoneImporter::Import(filepath, BoneImporter::ReferenceParameters{}).id;

	//!< animationの取り込み
	for (uint32_t i = 0; i < importer.GetAnimationCount(); ++i) {
		contents.animationIds.emplace_back(AnimationSequenceImporter::Import(filepath, AnimationSequenceImporter::ReferenceParameters{ .index = i }).id);
	}

	return contents;
}
