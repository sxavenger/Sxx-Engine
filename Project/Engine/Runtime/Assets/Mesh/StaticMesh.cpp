#include "StaticMesh.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [StaticMesh] Description structure
////////////////////////////////////////////////////////////////////////////////////////////

std::vector<uint32_t> StaticMesh::Description::GetIndices() const {
	std::vector<uint32_t> indices(polygons.size() * 3);
	std::memcpy(indices.data(), polygons.data(), polygons.size() * sizeof(MeshletPolygon)); //!< 連続配列なので, memcpyでコピー可能

	return indices;
}

void StaticMesh::Description::SetIndices(const std::vector<uint32_t>& indices) {
	StreamLogger::Assert(indices.size() % 3 == 0, "indices size must be triangle. (multiple of 3)");
	polygons = std::vector<MeshletPolygon>(indices.size() / 3); //!< 三角形の数に応じてpolygonsをリサイズ
	std::memcpy(polygons.data(), indices.data(), indices.size() * sizeof(uint32_t)); //!< 連続配列なので, memcpyでコピー可能
}
