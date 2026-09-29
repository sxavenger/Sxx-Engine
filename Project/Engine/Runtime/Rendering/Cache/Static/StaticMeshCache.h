#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cache
#include "BaseStaticCache.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/BottomLevelAccelerationStructure.h>
#include <Runtime/Assets/Mesh/StaticMesh.h>
#include <Runtime/Rendering/Meshlet/PositionVertexBuffer.h>
#include <Runtime/Rendering/Meshlet/StaticMeshVertexBuffer.h>
#include <Runtime/Rendering/Meshlet/TriangleIndexDimensionBuffer.h>
#include <Runtime/Rendering/Meshlet/MeshletBuffer.h>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// StaticMeshCache class
////////////////////////////////////////////////////////////////////////////////////////////
class StaticMeshCache final
	: public BaseStaticCache {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Cache(const std::shared_ptr<Assets::StaticMesh>& mesh);

	//* cache option *//

	const PositionVertexBuffer& GetPositionVertexBuffer() const { return positionVertexBuffer_; }

	const StaticMeshVertexBuffer& GetStaticMeshVertexBuffer() const { return staticMeshVertexBuffer_; }

	const TriangleIndexDimensionBuffer& GetIndexBuffer() const { return indexBuffer_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* Vertex Buffer *//

	PositionVertexBuffer positionVertexBuffer_;
	StaticMeshVertexBuffer staticMeshVertexBuffer_;

	TriangleIndexDimensionBuffer indexBuffer_;

	//* Acceleration Structure *//

	Graphics::BottomLevelAccelerationStructure bottomLevelAS_;

	//* Meshlet Buffer *//

	MeshletBuffer meshletBuffer_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static PositionVertexBuffer CreatePositionVertexBuffer(const std::string_view& name, const Assets::StaticMesh::Description& description);

	static StaticMeshVertexBuffer CreateStaticMeshVertexBuffer(const std::string_view& name, const Assets::StaticMesh::Description& description);

	static TriangleIndexDimensionBuffer CreateIndexBuffer(const std::string_view& name, const Assets::StaticMesh::Description& description);

	//* acceleration structure methods *//

	void BuildBottomLevelAccelerationStructure(const std::string_view& name);

	//* meshlet methods *//

	static MeshletBuffer CreateMeshletBuffer(const std::string_view& name, const Assets::StaticMesh::Description& description);

};

SXAVENGER_ENGINE_NAMESPACE_END
