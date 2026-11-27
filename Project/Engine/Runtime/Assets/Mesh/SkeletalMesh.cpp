#include "SkeletalMesh.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [SkeletalMesh] Description structure
////////////////////////////////////////////////////////////////////////////////////////////

std::span<const uint32_t> SkeletalMesh::Description::GetIndices() const {
	static_assert(sizeof(MeshPolygon) == sizeof(uint32_t) * 3, "MeshPolygon must be tightly packed 3 indices.");
	static_assert(alignof(MeshPolygon) == alignof(uint32_t), "MeshPolygon must have the same alignment as uint32_t.");

	return { reinterpret_cast<const uint32_t*>(polygons.data()), polygons.size() * 3 }; //!< 三角形の数 * 3
}

void SkeletalMesh::Description::SetIndices(const std::vector<uint32_t>& indices) {
	STREAM_ASSERT(indices.size() % 3 == 0, "indices size must be triangle. (multiple of 3)");
	polygons = std::vector<MeshPolygon>(indices.size() / 3); //!< 三角形の数に応じてpolygonsをリサイズ
	std::memcpy(polygons.data(), indices.data(), indices.size() * sizeof(uint32_t)); //!< 連続配列なので, memcpyでコピー可能
}

size_t SkeletalMesh::Description::GetIndexCount() const {
	return polygons.size() * 3; //!< 三角形の数 * 3
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletalMesh class methods
////////////////////////////////////////////////////////////////////////////////////////////

const SkeletalMesh::Description& SkeletalMesh::GetDescription() const {
	Scheduler::TaskHandle handle = BaseAsset::GetTaskHandle();

	if (handle != Scheduler::TaskState::State::Completed) {
		STREAM_LOG_WARNING("Assets::SkeletalMesh | skeletal mesh is not ready. name: {}, state: {}", GetName(), handle.GetState());
		handle.Wait(); //!< meshが準備できるまで待機
	}

	return description_;
}
