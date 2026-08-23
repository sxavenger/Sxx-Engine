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

//* meshoptimizer
#include <meshoptimizer/meshoptimizer.h>

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

	switch (metadata.GetType()) {
		case MetadataType::Reference:
			StaticMeshBuilder::BuildReference(mesh->description_, mesh->GetDirectory(), metadata.GetReferenceData());
			break;

		case MetadataType::Inline:
			StreamLogger::Error("Asset::StaticMeshBuilder | inline static mesh metadata type is not defined. filepath: {}", filepath.generic_string());
			break;

		default:
			StreamLogger::Error("Asset::StaticMeshBuilder | static mesh metadata type is unknown. filepath: {}", filepath.generic_string());
			return; //!< metadataがUnknownの場合は処理を終了
	}

	//!< meshの最適化
	StaticMeshBuilder::Optimize(mesh->description_);

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

void StaticMeshBuilder::Optimize(StaticMesh::Description& description) {

	//!< 頂点データの取得
	std::vector<MeshletVertex> vertices = description.vertices;
	std::vector<uint32_t> indices       = description.GetIndices();

	{ //!< 頂点データの重複頂点の削除

		//!< テーブルを作成
		std::vector<uint32_t> remap(indices.size());
		const size_t kUniqueVertexCount = meshopt_generateVertexRemap(
			remap.data(),
			indices.data(), indices.size(),
			vertices.data(), vertices.size(),
			sizeof(MeshletVertex)
		);

		std::vector<MeshletVertex> remappedVertices(kUniqueVertexCount);
		std::vector<uint32_t> remappedIndices(indices.size());

		//!< remapの適用

		meshopt_remapVertexBuffer(
			remappedVertices.data(),
			vertices.data(), vertices.size(),
			sizeof(MeshletVertex),
			remap.data()
		);

		meshopt_remapIndexBuffer(
			remappedIndices.data(),
			indices.data(), indices.size(),
			remap.data()
		);

		//!< descriptionの更新
		vertices = std::move(remappedVertices);
		indices  = std::move(remappedIndices);
	}

	//!< index順の最適化(in-placeでの更新)
	meshopt_optimizeVertexCache(
		indices.data(),
		indices.data(), indices.size(),
		vertices.size()
	);

	{ //!< 頂点順の最適化

		std::vector<MeshletVertex> fetchedVertices(vertices.size());
		const size_t kFetchedVertexCount = meshopt_optimizeVertexFetch(
			fetchedVertices.data(),
			indices.data(), indices.size(),
			vertices.data(), vertices.size(),
			sizeof(MeshletVertex)
		);
		fetchedVertices.resize(kFetchedVertexCount); //!< fetchされた頂点数にリサイズ

		//!< descriptionの更新
		vertices = std::move(fetchedVertices);
	}
	
	//!< descriptionの更新
	description.vertices = std::move(vertices);
	description.SetIndices(indices);
}
