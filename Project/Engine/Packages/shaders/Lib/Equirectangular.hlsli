#pragma once

#ifndef EQUIRECTANGULAR_LIBRARY_HLSLI
#define EQUIRECTANGULAR_LIBRARY_HLSLI

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Numeric.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Equirect namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Equirect {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief 正規化済みの方向ベクトルを正距円筒図法(Equirectangular)のテクスチャ座標に変換する
	//! @reference https://en.wikipedia.org/wiki/Spherical_coordinate_system
	float32_t2 DirectionToTexcoord(float32_t3 direction) {
		float32_t u = atan2(direction.z, direction.x) / Numeric::kTau + 0.5; //!< 経度
		float32_t v = acos(clamp(direction.y, -1.0, 1.0)) / Numeric::kPi;    //!< 緯度
		return float32_t2(u, v);
	}

	//! @brief 正距円筒図法(Equirectangular)のテクスチャ座標を正規化された方向ベクトルに変換する
	//! @reference https://en.wikipedia.org/wiki/Spherical_coordinate_system
	float32_t3 TexcoordToDirection(float32_t2 texcoord) {
		float32_t phi   = texcoord.x * Numeric::kTau - Numeric::kPi; //!< [-π, π]
		float32_t theta = texcoord.y * Numeric::kPi;                 //!< [0, π]
		float32_t s = sin(theta);
		return float32_t3(s * cos(phi), cos(theta), s * sin(phi));
	}

}

#endif
