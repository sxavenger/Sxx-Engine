#include "StaticMeshCache.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "../Meshlet/MeshletBuildOutput.h"

//* engine
#include <Runtime/Graphics/Core.h>
#include <Runtime/Scheduler/Common/TaskHandle.h>
#include <Runtime/Scheduler/System.h>

//* meshoptimizer
#include <meshoptimizer/meshoptimizer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// StaticMeshCache class methods
////////////////////////////////////////////////////////////////////////////////////////////

void StaticMeshCache::Cache(const std::shared_ptr<Assets::StaticMesh>& mesh) {

	Scheduler::TaskHandle handle = mesh->GetTaskHandle();

	if (handle.GetState() != Scheduler::TaskState::State::Completed) {
		STREAM_LOG_WARNING("Rendering::StaticMeshCache | static mesh is not ready. name: {}, state: {}", mesh->GetName(), handle.GetState());
		return; //!< taskが完了していない場合はキャッシュしない
	}

	//!< descriptionの取得
	const Assets::StaticMesh::Description& description = mesh->GetDescription();

	//!< 頂点bufferの作成
	positionVertexBuffer_   = StaticMeshCache::CreatePositionVertexBuffer(mesh->GetName(), description);
	staticMeshVertexBuffer_ = StaticMeshCache::CreateStaticMeshVertexBuffer(mesh->GetName(), description);
	indexBuffer_            = StaticMeshCache::CreateIndexBuffer(mesh->GetName(), description);

	//!< meshlet bufferの作成
	meshletBuffer_ = StaticMeshCache::CreateMeshletBuffer(mesh->GetName(), description);

	//!< BLASの構築
	BuildBottomLevelAccelerationStructure(mesh->GetName());

	//!< addressの取得
	address_ = mesh->GetAddress();

	STREAM_LOG_INFO("Rendering::StaticMeshCache | cache static mesh completed. name: {}", mesh->GetName());
}

PositionVertexBuffer StaticMeshCache::CreatePositionVertexBuffer(const std::string_view& name, const Assets::StaticMesh::Description& description) {

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

	buffer.positions.SetName(std::format("StaticMeshCache | PositionVertexBuffer | {}", name));
	return buffer;
}

StaticMeshVertexBuffer StaticMeshCache::CreateStaticMeshVertexBuffer(const std::string_view& name, const Assets::StaticMesh::Description& description) {

	//!< bufferの作成
	StaticMeshVertexBuffer buffer = StaticMeshVertexBuffer::Create(
		static_cast<UINT>(description.vertices.size()),
		1
	);

	//!< bufferの各頂点のmesh情報を設定
	for (size_t i = 0; i < description.vertices.size(); ++i) {
		StaticMeshVertexBuffer::Vertex& vertex = buffer.vertices.At(i);
		vertex.texcoord = description.vertices[i].texcoord;
		vertex.normal   = description.vertices[i].normal;
		vertex.tangent  = StaticMeshVertexBuffer::Vertex::EncodeTangent(
			description.vertices[i].normal,
			description.vertices[i].tangent,
			description.vertices[i].bitangent
		);
	}

	buffer.vertices.SetName(std::format("StaticMeshCache | StaticMeshVertexBuffer | {}", name));
	return buffer;
}

TriangleIndexDimensionBuffer StaticMeshCache::CreateIndexBuffer(const std::string_view& name, const Assets::StaticMesh::Description& description) {

	//!< bufferの作成
	TriangleIndexDimensionBuffer buffer = TriangleIndexDimensionBuffer::Create(
		static_cast<UINT>(description.polygons.size()),
		1
	);

	std::memcpy(buffer.GetIndexData(), description.polygons.data(), buffer.GetByteSize()); //!< 連続配列なので, memcpyでコピー可能

	buffer.SetName(std::format("StaticMeshCache | TriangleIndexBuffer | {}", name));
	return buffer;
}

void StaticMeshCache::BuildBottomLevelAccelerationStructure(const std::string_view& name) {

	Scheduler::TaskHandle task = Scheduler::System::PushComputeQueueTask(
		std::format("Rendering::StaticMeshCache | Build Bottom Level Acceleration Structure | {}", name),
		[this](Graphics::GraphicsCommandContext& context) {

			//!< geometryの作成
			Graphics::BottomLevelAccelerationStructure::Geometry geometry = Graphics::BottomLevelAccelerationStructure::Geometry::CreateTriangleGeometry(
				positionVertexBuffer_.positions.GetGpuVirtualAddress(),
				positionVertexBuffer_.positions.GetStride(),
				positionVertexBuffer_.positions.GetCapacity(),
				indexBuffer_.GetGpuVirtualAddress(),
				indexBuffer_.GetIndexCount(),
				Graphics::BottomLevelAccelerationStructure::Mode::Static
			);

			//!< BLASの構築
			bottomLevelAS_.Build(Graphics::Core::GetDevice(), context, geometry);

			context.SubmitWait(); //!< GPUの処理完了まで待機.
		}
	);
	// xxx: ダブルバッファリングの場合, Resourceが1つしかないので二重で実行してしまう.

	task.Wait(); //!< taskが完了するまで待機.

	bottomLevelAS_.SetName(std::format("StaticMeshCache | {}", name));
}

MeshletBuffer StaticMeshCache::CreateMeshletBuffer(const std::string_view& name, const Assets::StaticMesh::Description& description) {

	//!< 頂点データの取得
	std::span<const uint32_t> indices = description.GetIndices();

	//!< meshletの構築
	MeshletBuildOutput output = MeshletBuildOutput::Build(
		description.GetIndexCount(), indices.data(),
		description.vertices.size(), &description.vertices[0].position, sizeof(Assets::MeshVertex)
	);

	if (output.Empty()) {
		STREAM_LOG_ERROR("Rendering::StaticMeshCache | meshlet build failed. name: {}", name);
		return {};
	}

	//!< bufferの作成
	MeshletBuffer buffer;

	buffer.meshlets = Graphics::Core::CreateDimensionBuffer<MeshletBuffer::Meshlet>(
		static_cast<uint32_t>(output.meshlets.size()),
		1
	);
	std::memcpy(buffer.meshlets.GetData(), output.meshlets.data(), buffer.meshlets.GetByteSize());

	buffer.vertexIndices = Graphics::Core::CreateDimensionBuffer<uint32_t>(
		static_cast<uint32_t>(output.vertexIndices.size()),
		1
	);
	std::memcpy(buffer.vertexIndices.GetData(), output.vertexIndices.data(), buffer.vertexIndices.GetByteSize());

	buffer.triangles = Graphics::Core::CreateDimensionBuffer<MeshletBuffer::Triangle>(
		static_cast<uint32_t>(output.triangles.size()),
		1
	);
	std::memcpy(buffer.triangles.GetData(), output.triangles.data(), buffer.triangles.GetByteSize());

	buffer.bounds = Graphics::Core::CreateDimensionBuffer<MeshletBuffer::Bounds>(
		static_cast<uint32_t>(output.bounds.size()),
		1
	);
	std::memcpy(buffer.bounds.GetData(), output.bounds.data(), buffer.bounds.GetByteSize());

	buffer.SetName(name);

	return buffer;
}
