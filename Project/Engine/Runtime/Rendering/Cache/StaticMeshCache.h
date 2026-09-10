#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "../Meshlet/PositionVertexBuffer.h"
#include "../Meshlet/StaticMeshVertexBuffer.h"
#include "../Meshlet/TriangleIndexDimensionBuffer.h"
#include "../Meshlet/MeshletBuffer.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/BottomLevelAccelerationStructure.h>
#include <Runtime/Assets/Mesh/StaticMesh.h>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// StaticMeshCache class
////////////////////////////////////////////////////////////////////////////////////////////
class StaticMeshCache final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Cache(const std::shared_ptr<Assets::StaticMesh>& mesh);

	//* address option *//

	//! @brief chaceが有効かどうかを取得する.
	bool HasCache() const { return address_.has_value(); }

	//! @brief キャッシュされたアドレスを取得する.
	uintptr_t GetAddress() const { return address_.value_or(NULL); }

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

	//* address *//

	std::optional<uintptr_t> address_ = std::nullopt;

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
