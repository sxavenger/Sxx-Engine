#pragma once

// # reference
// [ACESlib.Transform_Common.ctl](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Transform_Common.ctl)

#ifndef ACES_TRANSFORM_COMMON_HLSLI
#define ACES_TRANSFORM_COMMON_HLSLI

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "UtilitiesColor.hlsli"
#include "Utilities.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// ACES namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ACES {

	//=========================================================================================
	// constant variables
	//=========================================================================================

	// --- AP0 / AP1 <-> XYZ --- //

	//!< AP0 / AP1 色度座標 - https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Transform_Common.ctl#L17-L24
	static const float32_t3x3 AP0_2_XYZ_MAT = RGBtoXYZ(AP0, 1.0);
	static const float32_t3x3 XYZ_2_AP0_MAT = XYZtoRGB(AP0, 1.0);

	static const float32_t3x3 AP1_2_XYZ_MAT = RGBtoXYZ(AP1, 1.0);
	static const float32_t3x3 XYZ_2_AP1_MAT = XYZtoRGB(AP1, 1.0);

	static const float32_t3x3 AP0_2_AP1_MAT = mul(AP0_2_XYZ_MAT, XYZ_2_AP1_MAT);
	static const float32_t3x3 AP1_2_AP0_MAT = mul(AP1_2_XYZ_MAT, XYZ_2_AP0_MAT);

	//!< AP1の輝度係数 - https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Transform_Common.ctl#L26-L28
	static const float32_t3 AP1_RGB2Y = float32_t3(
		AP1_2_XYZ_MAT[0][1],
		AP1_2_XYZ_MAT[1][1],
		AP1_2_XYZ_MAT[2][1]
	);

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @reference [rgb_2_saturation(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Transform_Common.ctl#L36-L39)
	float32_t rgb2saturation(float32_t3 rgb) {

		const float32_t TINY = 1e-10;

		float32_t min_rgb = min(min(rgb.x, rgb.y), rgb.z);
		float32_t max_rgb = max(max(rgb.x, rgb.y), rgb.z);
		return (max(max_rgb, TINY) - max(min_rgb, TINY)) / max(max_rgb, 1e-2);
	}

}

#endif
