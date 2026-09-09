#include "MeshletBuildOutput.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//* meshoptimizer
#include <meshoptimizer/meshoptimizer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshletBuildOutput structure methods
////////////////////////////////////////////////////////////////////////////////////////////

MeshletBuildOutput MeshletBuildOutput::Build(
	size_t indexCount, const uint32_t* indices,
	size_t vertexCount, const Vector3f* positions, size_t stride) {

	//!< meshletの最大個数を見積もり
	const size_t kMaxMeshletCount = meshopt_buildMeshletsBound(
		indexCount,
		MeshletBuffer::kMaxVertices,
		MeshletBuffer::kMaxTriangles
	);

	std::vector<meshopt_Meshlet> meshlets(kMaxMeshletCount);
	std::vector<uint32_t>        vertexIndices(kMaxMeshletCount * MeshletBuffer::kMaxVertices);
	std::vector<uint8_t>         primitives(kMaxMeshletCount * MeshletBuffer::kMaxTriangles * 3);

	//!< meshletの構築
	const size_t kMeshletCount = meshopt_buildMeshlets(
		meshlets.data(), vertexIndices.data(), primitives.data(),
		indices, indexCount,
		&positions->x, vertexCount, stride,
		MeshletBuffer::kMaxVertices,
		MeshletBuffer::kMaxTriangles,
		MeshletBuffer::kConeWeight
	);

	if (kMeshletCount == 0) {
		STREAM_LOG_ERROR("Rendering::MeshletBuildOutput | meshlet count is zero.");
		return {}; //!< meshletが構築できなかった場合は空のFooを返す
	}

	meshlets.resize(kMeshletCount); //!< 実際に使われた分まで切り詰める

	MeshletBuildOutput output;

	{ //!< 各情報を実際で使うサイズまでreserveしておく

		size_t totalVertexCount   = 0;
		size_t totalTriangleCount = 0;

		for (const auto& m : meshlets) {
			totalVertexCount   += m.vertex_count;
			totalTriangleCount += m.triangle_count;
		}

		output.meshlets.reserve(kMeshletCount);
		output.vertexIndices.reserve(totalVertexCount);
		output.triangles.reserve(totalTriangleCount);
		output.bounds.reserve(kMeshletCount);
		
	}

	for (size_t i = 0; i < kMeshletCount; ++i) {

		const meshopt_Meshlet& m = meshlets[i];

		//!< meshlet内の頂点/三角形の並びをlocality最適化
		meshopt_optimizeMeshlet(
			&vertexIndices[m.vertex_offset], &primitives[m.triangle_offset],
			m.triangle_count, m.vertex_count
		);

		const uint32_t vertexOffset   = static_cast<uint32_t>(output.vertexIndices.size());
		const uint32_t triangleOffset = static_cast<uint32_t>(output.triangles.size());

		//!< 頂点indexの詰め替え
		for (uint32_t j = 0; j < m.vertex_count; ++j) {
			output.vertexIndices.emplace_back(vertexIndices[m.vertex_offset + j]);
		}

		//!< 三角形indexの詰め替え
		for (uint32_t j = 0; j < m.triangle_count; ++j) {

			const uint32_t triangleIndex = m.triangle_offset + j * 3;

			MeshletBuffer::Triangle triangle = {};
			triangle.i0 = primitives[triangleIndex + 0];
			triangle.i1 = primitives[triangleIndex + 1];
			triangle.i2 = primitives[triangleIndex + 2];

			output.triangles.emplace_back(triangle);
		}

		
		{ //!< meshlet情報の詰め替え

			MeshletBuffer::Meshlet meshlet = {};
			meshlet.vertexOffset   = vertexOffset;
			meshlet.triangleOffset = triangleOffset;
			meshlet.vertexCount    = m.vertex_count;
			meshlet.triangleCount  = m.triangle_count;

			output.meshlets.emplace_back(meshlet);
		}

		{ //!< boundsを計算

			meshopt_Bounds b = meshopt_computeMeshletBounds(
				&vertexIndices[m.vertex_offset], &primitives[m.triangle_offset],
				m.triangle_count,
				&positions->x, vertexCount, stride
			);

			MeshletBuffer::Bounds bounds = {};
			bounds.center   = { b.center[0], b.center[1], b.center[2] };
			bounds.radius   = b.radius;
			bounds.coneAxis = { b.cone_axis_s8[0], b.cone_axis_s8[1], b.cone_axis_s8[2] };
			bounds.coneCutoff = b.cone_cutoff_s8;

			output.bounds.emplace_back(bounds);
		}
	}

	return output;
}
