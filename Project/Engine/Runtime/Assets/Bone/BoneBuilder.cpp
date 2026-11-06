#include "BoneBuilder.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Assimp/AssimpCommon.h"
#include "../Assimp/AssimpImporter.h"
#include "../Assimp/AssimpNode.h"
#include "../Importer/StaticMeshImporter.h"

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonFile.h>

////////////////////////////////////////////////////////////////////////////////////////////
// (anonymous namespace)
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	Bone::Description ReadNode(const std::filesystem::path& path, const AssimpNode& node) {

		//!< descriptionを設定
		Bone::Description description = {};
		description.name           = node.GetName();
		description.transformation = node.GetTransformation();

		for (uint32_t i = 0; i < node.GetChildCount(); ++i) {
			description.children.emplace_back(ReadNode(path, node.GetChild(i)));
		}

		for (uint32_t i = 0; i < node.GetMeshCount(); ++i) {
			Uuid mesh = StaticMeshImporter::Import(path, { .index = node.GetMeshIndex(i) });
			description.meshIds.emplace_back(mesh);
		}

		return description;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// BoneBuilder class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BoneBuilder::Build(std::shared_ptr<Bone>& bone) {

	//!< ファイルパスの取得
	const std::filesystem::path& filepath = bone->GetFilepath();

	if (!std::filesystem::exists(filepath)) {
		STREAM_LOG_ERROR("Assets::BoneBuilder | file does not exist. filepath: {}", filepath.generic_string());
		return; //!< ファイルが存在しない場合は処理を終了
	}

	//!< metadataの読み込み
	json::node node = JsonFile::Load(filepath);

	//!< metadataの設定
	const BoneMetadata& metadata = bone->metadata_ = BoneMetadata::Deserialize(node["metadata"]);

	switch (metadata.GetType()) {
		case MetadataType::Reference:
			bone->description_ = BoneBuilder::BuildReference(bone->GetDirectory(), metadata.GetReferenceData());
			break;

		case MetadataType::Inline:
			bone->description_ = {}; //!< TODO: Inline情報の定義.
			STREAM_LOG_ERROR("Assets::BoneBuilder | inline bone metadata type is not defined. filepath: {}", filepath.generic_string());
			return;

		default:
			STREAM_LOG_ERROR("Assets::BoneBuilder | bone metadata type is unknown. filepath: {}", filepath.generic_string());
			return; //!< metadataがUnknownの場合は処理を終了
	}

	STREAM_LOG_INFO("Assets::BoneBuilder | bone build completed. name: {}", bone->GetName());
}

Bone::Description BoneBuilder::BuildReference(const std::filesystem::path& directory, const BoneMetadata::ReferenceData& data) {

	//!< 参照されているboneのファイルパスの取得
	std::filesystem::path path = (directory / data.filepath).lexically_normal();

	//!< meshの取得
	const AssimpImporter importer = AssimpCommon::GetImporter(path);
	const AssimpNode root         = importer.GetRootNode();

	//!< descriptionの作成
	Bone::Description description = ReadNode(path, root);

	return description;
}
