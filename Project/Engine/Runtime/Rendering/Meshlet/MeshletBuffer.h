#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* meshlet
#include "Meshlet.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/DimensionBuffer.h>

//* lib
#include <Lib/Math/Vector3.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// MeshletBuffer structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MeshletBuffer final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* buffer option *//

	void SetName(const std::string_view& name) const;
	void SetName(const std::wstring_view& name) const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* buffer *//

	Graphics::DimensionBuffer<Meshlet::Description> meshlets;
	Graphics::DimensionBuffer<Meshlet::Triangle> triangles;
	Graphics::DimensionBuffer<uint32_t> vertexIndices;
	Graphics::DimensionBuffer<Meshlet::Bounds> bounds;

	//* parameter *//

	uint32_t meshletCount;

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
