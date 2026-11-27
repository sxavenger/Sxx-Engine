#include "SkeletalMeshIntermediateCache.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>
#include <Runtime/Scheduler/Common/TaskHandle.h>

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Logger/StackLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletalMeshIntermediateCache class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkeletalMeshIntermediateCache::Cache(const std::shared_ptr<const Assets::SkeletalMesh>& mesh) {

	Scheduler::TaskHandle handle = mesh->GetTaskHandle();

	if (handle.GetState() != Scheduler::TaskState::State::Completed) {
		STACK_LOG_WARNING("Rendering::SkeletalMeshIntermediateCache | skeletal mesh is not ready. name: {}", mesh->GetName());
		return; //!< taskが完了していない場合はキャッシュしない
	}

	//!< descriptionの取得
	const Assets::SkeletalMesh::Description& description = mesh->GetDescription();

	//!< 頂点bufferの作成
	positionVertexBuffer_ = SkeletalMeshIntermediateCache::CreatePositionVertexBuffer(mesh->GetName(), description);
	meshVertexBuffer_     = SkeletalMeshIntermediateCache::CreateMeshVertexBuffer(mesh->GetName(), description);
	indexBuffer_          = SkeletalMeshIntermediateCache::CreateIndexBuffer(mesh->GetName(), description);

	//!< influence bufferの作成
	influenceBuffer_ = SkeletalMeshIntermediateCache::CreateInfluenceBuffer(mesh->GetName(), description);

	//!< キャッシュの参照データの保持
	reference_ = mesh;
	address_   = mesh->GetAddress();

	STREAM_LOG_INFO("Rendering::SkeletalMeshIntermediateCache | cache skeletal mesh completed. name: {}", mesh->GetName());
}

std::shared_ptr<const Assets::SkeletalMesh> SkeletalMeshIntermediateCache::GetReference() const {
	return reference_.Get<std::shared_ptr<const Assets::SkeletalMesh>>(); //!< キャッシュの参照データを返す
}

uint32_t SkeletalMeshIntermediateCache::GetVertexCount() const {
	return static_cast<uint32_t>(positionVertexBuffer_.positions.GetCapacity());
}

uint32_t SkeletalMeshIntermediateCache::GetJointCount() const {
	return static_cast<uint32_t>(influenceBuffer_.influences.GetCapacity());
}

PositionVertexBuffer SkeletalMeshIntermediateCache::CreatePositionVertexBuffer(const std::string_view& name, const Assets::SkeletalMesh::Description& description) {

	//!< bufferの作成
	PositionVertexBuffer buffer = PositionVertexBuffer::Create(
		static_cast<UINT>(description.vertices.size()),
		1
	);

	//!< bufferの各頂点のpositionを設定
	for (size_t i = 0; i < description.vertices.size(); ++i) {
		Vector3f& position = buffer.positions.At(i);
		position = description.vertices[i].position;
	}

	buffer.positions.SetName(std::format("SkeletalMeshIntermediateCache | PositionVertexBuffer | {}", name));
	return buffer;
}

MeshVertexBuffer SkeletalMeshIntermediateCache::CreateMeshVertexBuffer(const std::string_view& name, const Assets::SkeletalMesh::Description& description) {

	//!< bufferの作成
	MeshVertexBuffer buffer = MeshVertexBuffer::Create(
		static_cast<UINT>(description.vertices.size()),
		1
	);

	//!< bufferの各頂点のmesh情報を設定
	for (size_t i = 0; i < description.vertices.size(); ++i) {
		MeshVertexBuffer::Vertex& vertex = buffer.vertices.At(i);
		vertex.texcoord = description.vertices[i].texcoord;
		vertex.normal   = description.vertices[i].normal;
		vertex.tangent  = MeshVertexBuffer::Vertex::EncodeTangent(
			description.vertices[i].normal,
			description.vertices[i].tangent,
			description.vertices[i].bitangent
		);
	}

	buffer.vertices.SetName(std::format("SkeletalMeshIntermediateCache | MeshVertexBuffer | {}", name));
	return buffer;
}

TriangleIndexDimensionBuffer SkeletalMeshIntermediateCache::CreateIndexBuffer(const std::string_view& name, const Assets::SkeletalMesh::Description& description) {

	//!< bufferの作成
	TriangleIndexDimensionBuffer buffer = TriangleIndexDimensionBuffer::Create(
		static_cast<UINT>(description.polygons.size()),
		1
	);

	std::memcpy(buffer.GetIndexData(), description.polygons.data(), buffer.GetByteSize()); //!< 連続配列なので, memcpyでコピー可能

	buffer.SetName(std::format("SkeletalMeshIntermediateCache | TriangleIndexBuffer | {}", name));
	return buffer;
}

MeshVertexInfluenceBuffer SkeletalMeshIntermediateCache::CreateInfluenceBuffer(const std::string_view& name, const Assets::SkeletalMesh::Description& description) {

	//!< influenceの作成
	std::vector<MeshVertexInfluenceBuffer::Influence> influences(description.vertices.size());
	for (uint32_t i = 0; i < description.joints.size(); ++i) {
		const Assets::SkeletalMesh::SkinJoint& joint = description.joints[i];

		for (const Assets::SkeletalMesh::SkinJoint::VertexWeight& weight : joint.weights) {
			if (weight.index >= influences.size()) {
				STACK_LOG_WARNING("Rendering::SkeletalMeshIntermediateCache | vertex weight index is out of range. index: {}, joint: {}", weight.index, joint.name);
				continue; //!< weight.indexが範囲外の場合はスキップ
			}

			if (weight.weight == 0.0f) {
				continue; //!< 影響のないweightは無視
			}

			//!< weightが最小のslotを探索 (未使用の場合は0.0が入っているので, それを使用する)
			auto min = std::min_element(influences[weight.index].weights.begin(), influences[weight.index].weights.end());

			if (weight.weight <= *min) {
				continue; //!< 既存のweightより小さい場合は無視
			}

			//!< slotに設定
			const size_t slot = static_cast<size_t>(std::distance(influences[weight.index].weights.begin(), min));
			influences[weight.index].weights[slot]      = weight.weight;
			influences[weight.index].jointIndices[slot] = i;
		}
	}

	//!< bufferの作成
	MeshVertexInfluenceBuffer buffer = MeshVertexInfluenceBuffer::Create(
		static_cast<uint32_t>(influences.size()),
		1
	);
	std::memcpy(buffer.influences.GetData(), influences.data(), buffer.influences.GetByteSize());

	buffer.influences.SetName(std::format("SkeletalMeshIntermediateCache | MeshVertexInfluenceBuffer | {}", name));
	return buffer;
}
