#pragma once

#ifndef ACES1_COLORSPACE_HLSLI
#define ACES1_COLORSPACE_HLSLI

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "UtilitiesColor.hlsli"
#include "TransformCommon.hlsli"
#include "RRT.hlsli"
#include "ODT.hlsli"
#include "OutputTransforms.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// ACES namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ACES {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief ACEScgの輝度を計算する | ACEScg -> Y
	float32_t CalcLuminance(float32_t3 acescg) {
		return dot(acescg, AP1_RGB2Y);
	}

	//! @brief Rec.709 (sRGB Linear) -> AP1 (ACEScg)
	float32_t3 Rec709ToAP1(float32_t3 rec709) {
		static const float32_t3x3 LinearSRGB_2_AP1_MAT = CalculateRGBToRGBMatrix(REC709_PRI, AP1);
		return mul(rec709, LinearSRGB_2_AP1_MAT);
	}

	//! @brief AP1 (ACEScg) -> ACES2065-1 (AP0)
	float32_t3 AP1ToAP0(float32_t3 ap1) {
		return mul(ap1, AP1_2_AP0_MAT);
	}

	// # Input Device Transform (IDT) Pipeline
	// - Input Texture | Rec709 Linear
	//  Texture<Rec709> -[Rec709ToAP1(...)]-> ACEScg<AP1>

	// # Output Device Transform (ODT) Pipeline
	// - Output SRGB Linear | Rec709 Linear
	//  ACEScg<AP1> -[AP1ToAP0(...)]-> ACES2065-1<AP0> -[RRTMain(...)]-> OCES -[ODTFwd(...)]-> linearCV
	// - Output SRGB | Rec709
	//  ACEScg<AP1> -[AP1ToAP0(...)]-> ACES2065-1<AP0> -[RRTMain(...)]-> OCES -[ODTFwd(...)]-> linearCV -[EOTFInv(...)]-> Rec709
}

#endif
