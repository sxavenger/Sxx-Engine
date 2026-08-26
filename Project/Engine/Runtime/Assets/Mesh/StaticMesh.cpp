#include "StaticMesh.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [StaticMesh] Description structure
////////////////////////////////////////////////////////////////////////////////////////////

std::vector<uint32_t> StaticMesh::Description::GetIndices() const {
	std::vector<uint32_t> indices(polygons.size() * 3);
	std::memcpy(indices.data(), polygons.data(), polygons.size() * sizeof(MeshPolygon)); //!< 連続配列なので, memcpyでコピー可能

	return indices;
}

void StaticMesh::Description::SetIndices(const std::vector<uint32_t>& indices) {
	STREAM_ASSERT(indices.size() % 3 == 0, "indices size must be triangle. (multiple of 3)");
	polygons = std::vector<MeshPolygon>(indices.size() / 3); //!< 三角形の数に応じてpolygonsをリサイズ
	std::memcpy(polygons.data(), indices.data(), indices.size() * sizeof(uint32_t)); //!< 連続配列なので, memcpyでコピー可能
}

const StaticMesh::Description& StaticMesh::GetDescription() const {
	Scheduler::TaskHandle handle = BaseAsset::GetTaskHandle();

	if (handle != Scheduler::TaskState::State::Completed) {
		STREAM_LOG_WARNING("Asset::StaticMesh | static mesh is not ready. name: {}, state: {}", GetName(), handle.GetState());
		handle.Wait(); //!< meshが準備できるまで待機
	}

	return description_;
}
