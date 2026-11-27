#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cache
#include "BaseStaticCache.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Assets/Mesh/SkeletalMesh.h>
#include <Runtime/Rendering/Meshlet/PositionVertexBuffer.h>
#include <Runtime/Rendering/Meshlet/MeshVertexBuffer.h>
#include <Runtime/Rendering/Meshlet/MeshVertexInfluenceBuffer.h>
#include <Runtime/Rendering/Meshlet/TriangleIndexDimensionBuffer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletalMeshIntermediateCache class
////////////////////////////////////////////////////////////////////////////////////////////
class SkeletalMeshIntermediateCache final
	: public BaseStaticCache {
	//!< 実際のSkinning等はComponentごとのDynamicCache側で行う
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Cache(const std::shared_ptr<const Assets::SkeletalMesh>& mesh);

	//* cache option *//

	std::shared_ptr<const Assets::SkeletalMesh> GetReference() const;

	uint32_t GetVertexCount() const;

	uint32_t GetJointCount() const;

	const PositionVertexBuffer& GetPositionVertexBuffer() const { return positionVertexBuffer_; }

	const MeshVertexBuffer& GetMeshVertexBuffer() const { return meshVertexBuffer_; }

	const TriangleIndexDimensionBuffer& GetIndexBuffer() const { return indexBuffer_; }

	const MeshVertexInfluenceBuffer& GetInfluenceBuffer() const { return influenceBuffer_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* Vertex Buffer *//

	PositionVertexBuffer positionVertexBuffer_;
	MeshVertexBuffer meshVertexBuffer_;

	TriangleIndexDimensionBuffer indexBuffer_;

	//* Influence Buffer *//

	MeshVertexInfluenceBuffer influenceBuffer_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static PositionVertexBuffer CreatePositionVertexBuffer(const std::string_view& name, const Assets::SkeletalMesh::Description& description);

	static MeshVertexBuffer CreateMeshVertexBuffer(const std::string_view& name, const Assets::SkeletalMesh::Description& description);

	static TriangleIndexDimensionBuffer CreateIndexBuffer(const std::string_view& name, const Assets::SkeletalMesh::Description& description);

	static MeshVertexInfluenceBuffer CreateInfluenceBuffer(const std::string_view& name, const Assets::SkeletalMesh::Description& description);

};

SXAVENGER_ENGINE_NAMESPACE_END
