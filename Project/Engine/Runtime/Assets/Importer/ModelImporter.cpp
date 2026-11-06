#include "ModelImporter.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Assimp/AssimpCommon.h"
#include "../Assimp/AssimpImporter.h"
#include "StaticMeshImporter.h"
#include "SurfaceMaterialImporter.h"
#include "BoneImporter.h"

//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelImporter class methods
////////////////////////////////////////////////////////////////////////////////////////////

ModelImporter::Contents ModelImporter::Import(const std::filesystem::path& filepath) {
	STREAM_ASSERT(std::filesystem::exists(filepath), "file does not exist. filepath: {}", filepath.generic_string()); //!< ファイルが存在するか確認
	STREAM_ASSERT(Assets::CheckFileType(filepath.extension()) == FileType::Other, "file is not a valid model file. filepath: {}", filepath.generic_string());

	//!< importerの取得
	const AssimpImporter& importer = AssimpCommon::GetImporter(filepath);

	const uint32_t meshCount     = importer.GetMeshCount();
	const uint32_t materialCount = importer.GetMaterialCount();

	ModelImporter::Contents output = {};
	output.materialIds.reserve(materialCount);
	output.meshIds.reserve(meshCount);

	//!< materialの取り込み
	for (uint32_t i = 0; i < materialCount; ++i) {
		SurfaceMaterialImporter::ReferenceParameters parameters = {};
		parameters.index = i;

		output.materialIds.emplace_back(SurfaceMaterialImporter::Import(filepath, parameters));
	}

	//!< meshの取り込み
	for (uint32_t i = 0; i < meshCount; ++i) {
		StaticMeshImporter::ReferenceParameters parameters = {};
		parameters.index = i;

		output.meshIds.emplace_back(StaticMeshImporter::Import(filepath, parameters));
	}

	//!< boneの取り込み
	output.boneId = BoneImporter::Import(filepath, BoneImporter::ReferenceParameters{});

	return output;
}
