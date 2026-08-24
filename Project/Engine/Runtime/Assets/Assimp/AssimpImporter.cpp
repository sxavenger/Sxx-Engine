#include "AssimpImporter.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//* assimp
#include <assimp/scene.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpImporter class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string_view AssimpImporter::GetErrorString() const {
	STREAM_ASSERT(importer_ != nullptr, "importer is null");
	return importer_->GetErrorString();
}

bool AssimpImporter::HasScene() const {
	STREAM_ASSERT(importer_ != nullptr, "importer is null");
	return importer_->GetScene() != nullptr;
}

uint32_t AssimpImporter::GetMeshCount() const {
	STREAM_ASSERT(HasScene(), "importer has no scene");
	return importer_->GetScene()->mNumMeshes;
}

AssimpMesh AssimpImporter::GetMesh(uint32_t index) const {
	STREAM_ASSERT(HasScene(), "importer has no scene");
	STREAM_ASSERT(index < GetMeshCount(), "index is out of bounds");
	return importer_->GetScene()->mMeshes[index];
}

AssimpImporter AssimpImporter::Load(const std::filesystem::path& filepath, FlagEnum<AssimpOption> option) {
	std::shared_ptr<Assimp::Importer> instance = std::make_shared<Assimp::Importer>();
	instance->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE); //!< 点と線のプリミティブを削除する
	instance->ReadFile(filepath.generic_string(), option.value);

	AssimpImporter importer = instance;

	STREAM_ASSERT_SUMMARY(importer.HasScene(), std::format("failed to assimp import. filepath: {}", filepath.generic_string()), importer.GetErrorString());
	return importer;
}
