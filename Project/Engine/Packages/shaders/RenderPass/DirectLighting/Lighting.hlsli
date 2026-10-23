#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* pass
#include "../PassContext.hlsli"
#include "GBuffer.hlsli"

//* system
#include "../../System/Semantics.hlsli"

//* lib
#include "../../Lib/BxDF/BRDF.hlsli"

//* component
#include "../../Component/Transform/Transform.hlsli"
#include "../../Component/Camera/Camera.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//! @brief コンテキスト情報
ConstantBuffer<PassContext> Context : register(b0, space1);

//! @brief 描画パス情報
cbuffer PassData : register(b1, space1) {
}

//! @brief camera情報
ConstantBuffer<Component::Camera> gCameraProjection   : register(b2, space1);
ConstantBuffer<Component::Transform> gCameraTransform : register(b3, space1);

//! @brief scene情報
RaytracingAccelerationStructure gScene : register(t0, space1);

//! @brief Gbuffer情報
Texture2D<float32_t> gDepth    : register(t1, space1);
Texture2D<float32_t4> gBufferA : register(t2, space1);
Texture2D<float32_t4> gBufferB : register(t3, space1);
Texture2D<float32_t4> gBufferC : register(t4, space1);

////////////////////////////////////////////////////////////////////////////////////////////
// PixelInputData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PixelRasterData {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float32_t4 position : SV_Position;
	uint32_t instance_id : INSTANCEID;

};

////////////////////////////////////////////////////////////////////////////////////////////
// PixelOutputData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PixelOutputData { //!< [--- 参照]

	//=========================================================================================
	// public variables
	//=========================================================================================

	float32_t4 buffer0 : SV_Target0; //!< FLOAT3 Diffuse(ACEScg) | FLOAT1 Unused

	//=========================================================================================
	// public methods
	//=========================================================================================

};

//-----------------------------------------------------------------------------------------
// constant variables
//-----------------------------------------------------------------------------------------

static const GBuffer::Textures kTextures = GBuffer::Textures::Create(gDepth, gBufferA, gBufferB, gBufferC); //!< GBufferのテクスチャ群
