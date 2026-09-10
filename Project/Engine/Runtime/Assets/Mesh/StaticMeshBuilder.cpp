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
		STREAM_LOG_ERROR("Asset::StaticMeshBuilder | file does not exist. filepath: {}", filepath.generic_string());
		return; //!< ファイルが存在しない場合は処理を終了
	}

	//!< metadataの読み込み
	json::node node = JsonFile::Load(filepath);

	//!< metadataの設定
	const StaticMeshMetadata& metadata = mesh->metadata_ = StaticMeshMetadata::Deserialize(node["metadata"]);

	switch (metadata.GetType()) {
		case MetadataType::Reference:
			mesh->description_ = StaticMeshBuilder::BuildReference(mesh->GetDirectory(), metadata.GetReferenceData());
			break;

		case MetadataType::Inline:
			mesh->description_ = {}; //!< TODO: Inline情報の定義.
			STREAM_LOG_ERROR("Asset::StaticMeshBuilder | inline static mesh metadata type is not defined. filepath: {}", filepath.generic_string());
			return;

		default:
			STREAM_LOG_ERROR("Asset::StaticMeshBuilder | static mesh metadata type is unknown. filepath: {}", filepath.generic_string());
			return; //!< metadataがUnknownの場合は処理を終了
	}

	//!< meshの最適化
	StaticMeshBuilder::Optimize(mesh->description_);

	STREAM_LOG_INFO("Asset::StaticMeshBuilder | static mesh build completed. name: {}", mesh->GetName());
}

StaticMesh::Description StaticMeshBuilder::BuildReference(const std::filesystem::path& directory, const StaticMeshMetadata::ReferenceData& data) {

	//!< 参照されているmeshのファイルパスの取得
	std::filesystem::path path = (directory / data.filepath).lexically_normal();

	//!< meshの取得
	const AssimpImporter importer = AssimpCommon::GetImporter(path);
	const AssimpMesh mesh         = importer.GetMesh(data.index);

	//!< descriptionの作成
	StaticMesh::Description description;
	description.vertices.resize(mesh.GetVertexCount());
	description.polygons.resize(mesh.GetFaceCount());

	//!< 頂点データの取得
	
	if (mesh.HasPosition()) {
		for (uint32_t i = 0; i < mesh.GetVertexCount(); ++i) {
			const Vector3f position = mesh.GetPosition(i);
			description.vertices[i].position = position;
		}
	}

	if (mesh.HasNormal()) {
		for (uint32_t i = 0; i < mesh.GetVertexCount(); ++i) {
			const Vector3f normal = mesh.GetNormal(i);
			description.vertices[i].normal = normal;
		}
	}

	if (mesh.HasTexcoord()) {
		for (uint32_t i = 0; i < mesh.GetVertexCount(); ++i) {
			const Vector2f texcoord = mesh.GetTexcoord(i);
			description.vertices[i].texcoord = texcoord;
		}
	}

	if (mesh.HasTangentBitangent()) {
		for (uint32_t i = 0; i < mesh.GetVertexCount(); ++i) {
			const Vector3f tangent   = mesh.GetTangent(i);
			const Vector3f bitangent = mesh.GetBitangent(i);
			description.vertices[i].tangent   = tangent;
			description.vertices[i].bitangent = bitangent;
		}
	}

	//!< 三角形データの取得
	for (uint32_t i = 0; i < mesh.GetFaceCount(); ++i) {
		const MeshPolygon polygon = { mesh.GetTriangle(i) };
		description.polygons[i] = polygon;
	}

	return description;
}

void StaticMeshBuilder::Optimize(StaticMesh::Description& description) {

	//!< 頂点データの取得
	std::vector<MeshVertex> vertices = description.vertices;

	std::span<const uint32_t> indicesView = description.GetIndices();
	std::vector<uint32_t> indices(indicesView.begin(), indicesView.end());

	{ //!< 頂点データの重複頂点の削除

		//!< テーブルを作成
		std::vector<uint32_t> remap(indices.size());
		const size_t kUniqueVertexCount = meshopt_generateVertexRemap(
			remap.data(),
			indices.data(), indices.size(),
			vertices.data(), vertices.size(),
			sizeof(MeshVertex)
		);

		std::vector<MeshVertex> remappedVertices(kUniqueVertexCount);
		std::vector<uint32_t> remappedIndices(indices.size());

		//!< remapの適用

		meshopt_remapVertexBuffer(
			remappedVertices.data(),
			vertices.data(), vertices.size(), sizeof(MeshVertex),
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

		std::vector<MeshVertex> fetchedVertices(vertices.size());
		const size_t kFetchedVertexCount = meshopt_optimizeVertexFetch(
			fetchedVertices.data(),
			indices.data(), indices.size(),
			vertices.data(), vertices.size(), sizeof(MeshVertex)
		);
		fetchedVertices.resize(kFetchedVertexCount); //!< fetchされた頂点数にリサイズ

		//!< descriptionの更新
		vertices = std::move(fetchedVertices);
	}
	
	//!< descriptionの更新
	description.vertices = std::move(vertices);
	description.SetIndices(indices);
}
