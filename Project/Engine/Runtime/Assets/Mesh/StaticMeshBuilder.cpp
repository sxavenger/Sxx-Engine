#include "StaticMeshBuilder.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Assimp/AssimpCommon.h"
#include "../Assimp/AssimpImporter.h"
#include "../Assimp/AssimpMesh.h"

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonFile.h>

////////////////////////////////////////////////////////////////////////////////////////////
// StaticMeshBuilder class methods
////////////////////////////////////////////////////////////////////////////////////////////

void StaticMeshBuilder::Build(std::shared_ptr<StaticMesh>& mesh) {

	//!< ファイルパスの取得
	const std::filesystem::path& filepath = mesh->GetFilepath();

	if (!std::filesystem::exists(filepath)) {
		StreamLogger::Error("Asset::StaticMeshBuilder | file does not exist. filepath: {}", filepath.generic_string());
		return; //!< ファイルが存在しない場合は処理を終了
	}


	//!< metadataの読み込み
	json::node node = JsonFile::Load(filepath);

	//!< metadataの設定
	const StaticMeshMetadata& metadata = mesh->metadata_ = StaticMeshMetadata::Deserialize(node["metadata"]);

	switch (mesh->metadata_.GetType()) {
		case MetadataType::Reference:
			StaticMeshBuilder::BuildReference(mesh->description_, mesh->GetDirectory(), mesh->metadata_.GetReferenceData());
			break;

		case MetadataType::Inline:
			StreamLogger::Error("Asset::StaticMeshBuilder | inline static mesh metadata type is not defined. filepath: {}", filepath.generic_string());
			break;

		default:
			StreamLogger::Error("Asset::StaticMeshBuilder | static mesh metadata type is unknown. filepath: {}", filepath.generic_string());
			return; //!< metadataがUnknownの場合は処理を終了
	}

	StreamLogger::Info("Asset::StaticMeshBuilder | static mesh build completed. name: {}", mesh->GetName());
}

void StaticMeshBuilder::BuildReference(StaticMesh::Description& description, const std::filesystem::path& directory, const StaticMeshMetadata::ReferenceData& data) {

	//!< 参照されているmeshのファイルパスの取得
	std::filesystem::path path = (directory / data.filepath).lexically_normal();

	//!< meshの取得
	const AssimpImporter importer = AssimpCommon::GetImporter(path);
	const AssimpMesh mesh         = importer.GetMesh(data.index);

	//!< 頂点データの取得
	for (uint32_t i = 0; i < mesh.GetVertexCount(); ++i) {

		MeshletVertex vertex = {};

		if (mesh.HasPosition()) {
			vertex.position = mesh.GetPosition(i);
		}

		if (mesh.HasNormal()) {
			vertex.normal = mesh.GetNormal(i);
		}

		if (mesh.HasTexcoord()) {
			vertex.texcoord = mesh.GetTexcoord(i);
		}

		if (mesh.HasTangentBitangent()) {
			vertex.tangent   = mesh.GetTangent(i);
			vertex.bitangent = mesh.GetBitangent(i);
		}

		description.vertices.emplace_back(vertex);
	}

	//!< 三角形データの取得
	for (uint32_t i = 0; i < mesh.GetFaceCount(); ++i) {

		MeshletPolygon polygon = {};

		polygon.indices = mesh.GetTriangle(i);

		description.polygons.emplace_back(polygon);
	}

}
