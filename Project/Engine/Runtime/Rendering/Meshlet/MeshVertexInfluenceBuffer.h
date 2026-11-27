#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* meshlet
#include "PackedNormal.h"
#include "PackedTexcoord.h"
#include "VertexDimensionBuffer.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// MeshVertexInfluenceBuffer structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MeshVertexInfluenceBuffer final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Influence structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Influence final {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		static const uint32_t kInfluenceCount = 4; //!< 1頂点あたりの影響を受けるジョイント数

		std::array<uint32_t, kInfluenceCount> jointIndices = { 0, 0, 0, 0 };
		std::array<float,    kInfluenceCount> weights      = { 0.0f, 0.0f, 0.0f, 0.0f };

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static MeshVertexInfluenceBuffer Create(uint32_t capacity, uint8_t frameCount);

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* Buffer *//

	VertexDimensionBuffer<Influence> influences;

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
