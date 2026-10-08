#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* system
#include "../../System/Semantics.hlsli"

//* meshlet
#include "../../Meshlet/Meshlet.hlsli"
#include "../../Meshlet/StaticMeshVertex.hlsli"

//* component
#include "../../Component/Camera.hlsli"
#include "../../Component/Transform.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

cbuffer Dimension : register(b0, space1) {
	uint32_t2 dimension;
}

cbuffer Information : register(b1, space1) {
	uint32_t meshlet_count;
}

StructuredBuffer<float32_t3> gPositions : register(t0, space1);
StructuredBuffer<StaticMeshVertex> gVertices : register(t1, space1);
StructuredBuffer<Meshlet> gMeshlets : register(t2, space1);
StructuredBuffer<uint32_t> gVertexIndices : register(t3, space1);
StructuredBuffer<MeshletTriangle> gTriangles : register(t4, space1);

////////////////////////////////////////////////////////////////////////////////////////////
// intermediate structures
////////////////////////////////////////////////////////////////////////////////////////////

struct PixelRasterData {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float32_t4 position : SV_Position;

	uint32_t meshlet_index : MESHLET_INDEX;

};

struct PixelInputData {

	//=========================================================================================
	// public variables
	//=========================================================================================

	PixelRasterData raster;

	float32_t3 position : POSITION0;
	float32_t3 normal : NORMAL0;
	float32_t3 tangent : TANGENT0;
	float32_t3 bitangent : BITANGENT0;
	float32_t2 texcoord : TEXCOORD0;

	//=========================================================================================
	// public methods
	//=========================================================================================

};

struct PixelOutputData {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float4 color : SV_Target0;

};
