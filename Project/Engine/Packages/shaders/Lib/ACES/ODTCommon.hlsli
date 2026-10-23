#pragma once

// # reference
// [ACESlib.ODT_Common.ctl](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.ODT_Common.ctl)

#ifndef ACES_ODT_COMMON_HLSLI
#define ACES_ODT_COMMON_HLSLI

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "TransformCommon.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// ACES namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ACES {

	//=========================================================================================
	// constant variables
	//=========================================================================================

	// Target white and black points for cinema system tonescale - https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.ODT_Common.ctl#L12-L22
	static const float32_t CINEMA_WHITE = 48.0;
	static const float32_t CINEMA_BLACK = pow(10.0, log10(0.02));

	// Gamma compensation factor - https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.ODT_Common.ctl#L27-L28
	static const float32_t DIM_SURROUND_GAMMA = 0.9811;

	// Saturation compensation factor - https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.ODT_Common.ctl#L30-L32
	static const float32_t ODT_SAT_FACTOR = 0.93;
	static const float32_t3x3 ODT_SAT_MAT = CalcSatAdjustMatrix(ODT_SAT_FACTOR, AP1_RGB2Y);

	// ACES の白 (D60 近傍) -> D65 (Bradford) - https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.ODT_Common.ctl#L37-L37
	static const float32_t3x3 D60_2_D65_CAT = CalculateCatMatrix(AP0.white, REC709_PRI.white);

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	// --- Y <-> linCV / surround 補正 --- //

	//! @reference [Y_2_linCV(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.ODT_Common.ctl#L42-L45)
	float32_t YToLinCV(float32_t Y, float32_t Ymax, float32_t Ymin) {
		return (Y - Ymin) / (Ymax - Ymin);
	}

	//! @reference [linCV_2_Y(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.ODT_Common.ctl#L47-L50)
	float32_t LinCVToY(float32_t linCV, float Ymax, float Ymin) {
		return linCV * (Ymax - Ymin) + Ymin;
	}

	//! @reference [Y_2_linCV_f3(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.ODT_Common.ctl#L52-L59)
	float32_t3 YToLinCV(float32_t3 Y, float32_t Ymax, float32_t Ymin) {
		return float32_t3(
			YToLinCV(Y.r, Ymax, Ymin),
			YToLinCV(Y.g, Ymax, Ymin),
			YToLinCV(Y.b, Ymax, Ymin)
		);
	}

	//! @reference [linCV_2_Y_f3(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.ODT_Common.ctl#L61-L68)
	float32_t3 LinCVToY(float32_t3 linCV, float32_t Ymax, float32_t Ymin) {
		return float32_t3(
			LinCVToY(linCV.r, Ymax, Ymin),
			LinCVToY(linCV.g, Ymax, Ymin),
			LinCVToY(linCV.b, Ymax, Ymin)
		);
	}

	//! @reference [darkSurround_to_dimSurround(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.ODT_Common.ctl#L70-L80)
	float32_t3 DarkSurroundToDimSurround(float32_t3 linearCV) {
		// dark surround 向けの値を dim surround 向けに補正する. 入出力は AP1 の linear code value.

		float32_t3 XYZ = mul(linearCV, AP1_2_XYZ_MAT);

		float32_t3 xyY = XYZ2xyY(XYZ);
		xyY.z = max(xyY.z, 0.0); // clamp(xyY[2], 0., HALF_POS_INF)
		xyY.z = pow(xyY.z, DIM_SURROUND_GAMMA);
		XYZ = xyY2XYZ(xyY);

		return mul(XYZ, XYZ_2_AP1_MAT);
	}

	//! @reference [dimSurround_to_darkSurround(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.ODT_Common.ctl#L82-L92)
	float32_t3 DimSurroundToDarkSurround(float32_t3 linearCV) {
		// dim surround 向けの値を dark surround 向けに補正する. 入出力は AP1 の linear code value.

		float32_t3 XYZ = mul(linearCV, AP1_2_XYZ_MAT);

		float32_t3 xyY = XYZ2xyY(XYZ);
		xyY.z = max(xyY.z, 0.0); // clamp(xyY[2], 0., HALF_POS_INF)
		xyY.z = pow(xyY.z, 1.0 / DIM_SURROUND_GAMMA);
		XYZ = xyY2XYZ(xyY);

		return mul(XYZ, XYZ_2_AP1_MAT);
	}

}

#endif
