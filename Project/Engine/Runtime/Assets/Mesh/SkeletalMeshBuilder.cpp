#include "SkeletalMeshBuilder.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Assimp/AssimpCommon.h"
#include "../Assimp/AssimpImporter.h"
#include "../Assimp/AssimpMesh.h"
#include "../Assimp/AssimpBone.h"
#include "../Importer/SurfaceMaterialImporter.h"

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonFile.h>

//* meshoptimizer
#include <meshoptimizer/meshoptimizer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletalMeshBuilder class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkeletalMeshBuilder::Build(std::shared_ptr<SkeletalMesh>& mesh) {

	//!< ファイルパスの取得
	const std::filesystem::path& filepath = mesh->GetFilepath();

	if (!std::filesystem::exists(filepath)) {
		STREAM_LOG_ERROR("Assets::SkeletalMeshBuilder | file does not exist. filepath: {}", filepath.generic_string());
		return; //!< ファイルが存在しない場合は処理を終了
	}

	//!< metadataの読み込み
	json::node node = JsonFile::Load(filepath);

	//!< metadataの設定
	const SkeletalMeshMetadata& metadata = mesh->metadata_ = SkeletalMeshMetadata::Deserialize(node["metadata"]);

	switch (metadata.GetType()) {
		case MetadataType::Reference:
			mesh->description_ = SkeletalMeshBuilder::BuildReference(mesh->GetDirectory(), metadata.GetReferenceData());
			break;

		case MetadataType::Inline:
			mesh->description_ = {}; //!< TODO: Inline情報の定義.
			STREAM_LOG_ERROR("Assets::SkeletalMeshBuilder | inline skeletal mesh metadata type is not defined. filepath: {}", filepath.generic_string());
			return;

		default:
			STREAM_LOG_ERROR("Assets::SkeletalMeshBuilder | skeletal mesh metadata type is unknown. filepath: {}", filepath.generic_string());
			return; //!< metadataがUnknownの場合は処理を終了
	}

	STREAM_LOG_INFO("Assets::SkeletalMeshBuilder | skeletal mesh build completed. name: {}", mesh->GetName());
}

SkeletalMesh::Description SkeletalMeshBuilder::BuildReference(const std::filesystem::path& directory, const SkeletalMeshMetadata::ReferenceData& data) {

	//!< 参照されているmeshのファイルパスの取得
	std::filesystem::path path = (directory / data.filepath).lexically_normal();

	//!< meshの取得
	const AssimpImporter importer = AssimpCommon::GetImporter(path);
	const AssimpMesh mesh         = importer.GetMesh(data.index);

	//!< descriptionの作成
	SkeletalMesh::Description description;
	description.vertices.resize(mesh.GetVertexCount());
	description.polygons.resize(mesh.GetFaceCount());
	description.joints.resize(mesh.GetBoneCount());

	//!< 頂点データの取得
	
	if (mesh.HasPosition()) {
		for (uint32_t i = 0; i < mesh.GetVertexCount(); ++i) {
			description.vertices[i].position = mesh.GetPosition(i);
		}
	}

	if (mesh.HasNormal()) {
		for (uint32_t i = 0; i < mesh.GetVertexCount(); ++i) {
			description.vertices[i].normal = mesh.GetNormal(i);
		}
	}

	if (mesh.HasTexcoord()) {
		for (uint32_t i = 0; i < mesh.GetVertexCount(); ++i) {
			description.vertices[i].texcoord = mesh.GetTexcoord(i);
		}
	}

	if (mesh.HasTangentBitangent()) {
		for (uint32_t i = 0; i < mesh.GetVertexCount(); ++i) {
			description.vertices[i].tangent   = mesh.GetTangent(i);
			description.vertices[i].bitangent = mesh.GetBitangent(i);
		}
	}

	//!< 三角形データの取得
	for (uint32_t i = 0; i < mesh.GetFaceCount(); ++i) {
		AssimpMesh::Triangle triangle = mesh.GetTriangle(i);
		description.polygons[i].triangles[0] = triangle[0];
		description.polygons[i].triangles[1] = triangle[1];
		description.polygons[i].triangles[2] = triangle[2];
	}

	//!< SkinClusterの取得
	for (uint32_t i = 0; i < mesh.GetBoneCount(); ++i) {

		const AssimpBone bone = mesh.GetBone(i);

		SkeletalMesh::SkinJoint joint;

		//!< joint名の取得
		joint.name = bone.GetName();

		//!< bindPoseの取得
		joint.bindPoseInverse = bone.GetOffsetTransformation(); //!< bindPoseInverseの取得(内部でInverseされている)

		//!< weightsの取得
		for (uint32_t j = 0; j < bone.GetVertexWeightCount(); ++j) {
			AssimpBone::VertexWeight weight = bone.GetVertexWeight(j);
			joint.weights.emplace_back(weight.weight, weight.index);
		}

		description.joints[i] = std::move(joint); //!< jointの設定
	}

	return description;
}
