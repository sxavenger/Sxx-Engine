#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "MeshletBuffer.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Vector3.h>

//* c++
#include <cstdint>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// MeshletBuildOutput structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MeshletBuildOutput {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* meshlet option *//

	bool Empty() const { return meshlets.empty(); }

	//* static build method *//

	//! @brief meshoptimizerを使用してMeshletを構築する.
	static MeshletBuildOutput Build(
		size_t indexCount, const uint32_t* indices,
		size_t vertexCount, const Vector3f* positions, size_t stride
	);

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::vector<MeshletBuffer::Meshlet>  meshlets;
	std::vector<uint32_t>                vertexIndices;
	std::vector<MeshletBuffer::Triangle> triangles;
	//std::vector<MeshletBuffer::Bounds>   bounds;

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
