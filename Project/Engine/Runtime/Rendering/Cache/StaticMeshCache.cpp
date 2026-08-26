#include "StaticMeshCache.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>
#include <Runtime/Scheduler/Common/TaskHandle.h>
#include <Runtime/Scheduler/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// StaticMeshCache class methods
////////////////////////////////////////////////////////////////////////////////////////////

void StaticMeshCache::Cache(const std::shared_ptr<Assets::StaticMesh>& mesh) {

	Scheduler::TaskHandle handle = mesh->GetTaskHandle();

	if (handle.GetState() != Scheduler::TaskState::State::Completed) {
		return; //!< taskが完了していない場合はキャッシュしない
	}

	//!< descriptionの取得
	const Assets::StaticMesh::Description& description = mesh->GetDescription();

	//!< bufferの作成
	positionVertexBuffer_   = StaticMeshCache::CreatePositionVertexBuffer(mesh->GetName(), description);
	staticMeshVertexBuffer_ = StaticMeshCache::CreateStaticMeshVertexBuffer(mesh->GetName(), description);
	indexBuffer_            = StaticMeshCache::CreateIndexBuffer(mesh->GetName(), description);

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

	task.Wait(); //!< taskが完了するまで待機.

	bottomLevelAS_.SetName(std::format("StaticMeshCache | Bottom Level Acceleration Structure | {}", name));
}
