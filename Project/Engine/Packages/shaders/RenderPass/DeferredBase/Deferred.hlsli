#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* system
#include "../../System/Semantics.hlsli"

//* meshlet
#include "../../Meshlet/StaticMeshVertex.hlsli"
#include "../../Meshlet/Meshlet.hlsli"

//* component
#include "../../Component/Transform.hlsli"
#include "../../Component/Camera.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//! @brief コンテキスト情報
cbuffer Context : register(b0, space1) {
	uint32_t2 resolution;
}

//! @brief 描画パス情報
cbuffer PassData : register(b1, space1) {
	float32_t alpha_cutoff;
}

//! @brief 頂点情報
StructuredBuffer<float32_t3> gPositions      : register(t0, space1);
StructuredBuffer<StaticMeshVertex> gVertices : register(t1, space1);

//! @brief メッシュレット情報 (vertex shaderの場合は不要)
cbuffer MeshletData : register(b2, space1) {
	uint32_t meshlet_count;
}
StructuredBuffer<Meshlet::Description> gMeshlets : register(t2, space1);
StructuredBuffer<uint32_t> gVertexIndices        : register(t3, space1);
StructuredBuffer<Meshlet::Triangle> gTriangles   : register(t4, space1);

//! @brief object情報
ConstantBuffer<Component::Transform> gTransform : register(b3, space1);

//! @brief camera情報
ConstantBuffer<Component::Camera> gCameraProjection   : register(b4, space1);
ConstantBuffer<Component::Transform> gCameraTransform : register(b5, space1);

////////////////////////////////////////////////////////////////////////////////////////////
// PixelInputData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PixelInputData {

	////////////////////////////////////////////////////////////////////////////////////////////
	// RasterData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RasterData {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float32_t4 position : SV_Position;

		uint32_t meshlet_index : MESHLET_INDEX;

	};

	//=========================================================================================
	// public variables
	//=========================================================================================

	RasterData raster;

	float32_t3 position  : POSITION0;
	float32_t3 normal    : NORMAL0;
	float32_t3 tangent   : TANGENT0;
	float32_t3 bitangent : BITANGENT0;
	float32_t2 texcoord  : TEXCOORD0;

};

////////////////////////////////////////////////////////////////////////////////////////////
// PixelOutputData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PixelOutputData { //!< [Rendering::GBuffer 参照]

	//=========================================================================================
	// public variables
	//=========================================================================================

	float32_t4 buffer0 : SV_Target0; //!< FLOAT3 Albedo(sRGB) | FLOAT1 Unused
	float32_t4 buffer1 : SV_Target1; //!< FLOAT3 WorldNormal | FLOAT1 Unused
	float32_t4 buffer2 : SV_Target2; //!< FLOAT1 AmbientOcclusion | FLOAT1 Roughness | FLOAT1 Metallic | FLOAT1 Unused

	//=========================================================================================
	// public methods
	//=========================================================================================

	void SetAlbedo(float32_t3 albedo) {
		buffer0.rgb = albedo;
		buffer0.a   = 1.0;
	}

	void SetNormal(float32_t3 normal) {
		buffer1.rgb = normal * 0.5 + 0.5; //!< [-1, 1] -> [0, 1] に変換
		buffer1.a   = 1.0;
	}

	void SetRoughness(float32_t roughness) {
		buffer2.g = roughness;
		buffer2.a = 1.0;
	}

	void SetMetallic(float32_t metallic) {
		buffer2.b = metallic;
		buffer2.a = 1.0;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// methods structure
////////////////////////////////////////////////////////////////////////////////////////////

PixelInputData BuildPixelInputData(float32_t3 local_position, StaticMeshVertex vertex) {

	float32_t3 world_position = gTransform.TransformPoint(local_position);

	PixelInputData output = (PixelInputData)0;

	//!< 頂点情報の登録
	output.position  = world_position;
	output.normal    = gTransform.TransformNormal(vertex.GetNormal());
	output.tangent   = gTransform.TransformNormal(vertex.GetTangent());
	output.bitangent = gTransform.TransformNormal(vertex.GetBitangent());
	output.texcoord  = vertex.GetTexcoord();

	//!< raster情報の登録
	output.raster.position = mul(float32_t4(world_position, 1.0), mul(gCameraTransform.GetView(), gCameraProjection.GetProjection()));

	return output;
}
