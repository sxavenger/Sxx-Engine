#pragma once

// # reference
// [ACESlib.OutputTransforms.ctl](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.OutputTransforms.ctl)

#ifndef ACES_OUTPUT_TRANSFORMS_HLSLI
#define ACES_OUTPUT_TRANSFORMS_HLSLI

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "TransformCommon.hlsli"
#include "RRTCommon.hlsli"
#include "ODTCommon.hlsli"
#include "ODT.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// ACES namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ACES {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief EOTF forward function | コード値 -> linearCV
	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.OutputTransforms.ctl#L253-L283
	float32_t3 EOTFFwd(float32_t3 outputCV, EOTF eotf, float32_t Y_MAX, float32_t Y_MIN) {

		switch (eotf) {
			case EOTF::ST2084:
				return ST2084ToY(outputCV) / Y_MAX;

			case EOTF::BT1886:
				return BT1886Fwd(outputCV, 2.4, 1.0, 0.0);

			case EOTF::sRGB:
				{
					const float32_t DISPGAMMA = 2.4;
					const float32_t OFFSET    = 0.055;
					return MoncurveFwd(outputCV, DISPGAMMA, OFFSET);
				}

			case EOTF::Gamma_2_6:
				return pow(outputCV, 2.6);

			case EOTF::Linear:
				return (outputCV - Y_MIN) / (Y_MAX - Y_MIN);

			case EOTF::HLG:
				return ST2084ToY(HLGToST20841000nits(outputCV)) / Y_MAX;
		}

		return outputCV; // default: no EOTF
	}


	//! @brief EOTF inverse function | linearCV -> コード値
	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.OutputTransforms.ctl#L176-L213
	float32_t3 EOTFInv(float32_t3 linearCV, EOTF eotf, float32_t Y_MAX, float32_t Y_MIN) {

		// Clip values < 0 (i.e. projecting outside the display primaries)
		linearCV = max(linearCV, 0.0); // clamp(linearCV, 0., HALF_POS_INF)

		switch (eotf) {
			case EOTF::ST2084:
				return YToST2084(max(linearCV * Y_MAX, 0.0));

			case EOTF::BT1886:
				return BT1886Rev(linearCV, 2.4, 1.0, 0.0);

			case EOTF::sRGB:
				{
					const float32_t DISPGAMMA = 2.4;
					const float32_t OFFSET    = 0.055;
					return MoncurveRev(linearCV, DISPGAMMA, OFFSET);
				}

			case EOTF::Gamma_2_6:
				return pow(linearCV, 1.0 / 2.6);

			case EOTF::Linear:
				return linearCV * (Y_MAX - Y_MIN) + Y_MIN;

			case EOTF::HLG:
				return ST2084ToHLG1000nits(YToST2084(max(linearCV * Y_MAX, 0.0)));
		}

		return linearCV; // default: no EOTF
	}

}

#endif
