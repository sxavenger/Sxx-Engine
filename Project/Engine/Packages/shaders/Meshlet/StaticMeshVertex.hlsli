#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include "../Lib/Numeric.hlsli"

//* meshlet
#include "PackedNormal.hlsli"
#include "PackedTexcoord.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// StaticMeshVertex structure
////////////////////////////////////////////////////////////////////////////////////////////
struct StaticMeshVertex {

	//=========================================================================================
	// public variables
	//=========================================================================================

	PackedNormal normal;
	PackedNormal tangent;
	PackedTexcoord texcoord;

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief normalを取得.
	float32_t3 GetNormal() {
		return normalize(normal.Unpack3());
	}

	//! @brief tangentを取得.
	//! @note 頂点補間と量子化で崩れた直交性をGram-Schmidtで再構築した, local spaceの単位tangent.
	float32_t3 GetTangent() {
		float32_t3 n = GetNormal();
		float32_t3 t = tangent.Unpack3();

		float32_t3 rejection = t - n * dot(n, t);

		return rejection * rsqrt(max(dot(rejection, rejection), Numeric::kEpsilon));
	}

	//! @brief bitangentを取得.
	float32_t3 GetBitangent() {
		float32_t3 n = GetNormal();
		float32_t3 t = GetTangent();

		return cross(n, t) * tangent.Unpack4().w;
	}

	//! @brief texcoordを取得.
	float32_t2 GetTexcoord() {
		return texcoord.Unpack2();
	}

};
