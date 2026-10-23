#pragma once

// # reference
// [ACESlib.RRT_Common.ctl](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.RRT_Common.ctl)

#ifndef ACES_RRT_COMMON_HLSLI
#define ACES_RRT_COMMON_HLSLI

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

	//!< "Glow" module constants - https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.RRT_Common.ctl#L11-L13
	static const float32_t RRT_GLOW_GAIN = 0.05;
	static const float32_t RRT_GLOW_MID  = 0.08;

	//!< Red modifier constants - https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.RRT_Common.ctl#L15-L19
	static const float32_t RRT_RED_SCALE = 0.82;
	static const float32_t RRT_RED_PIVOT = 0.03;
	static const float32_t RRT_RED_HUE   = 0.0;
	static const float32_t RRT_RED_WIDTH = 135.0;

	//!< Desaturation constants - https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.RRT_Common.ctl#L21-L23
	static const float32_t RRT_SAT_FACTOR = 0.96;
	static const float32_t3x3 RRT_SAT_MAT = CalcSatAdjustMatrix(RRT_SAT_FACTOR, AP1_RGB2Y);

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	// --- Glow module --- //

	//! @reference [glow_fwd(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.RRT_Common.ctl#L29-L42)
	float32_t GlowFwd(float32_t ycIn, float32_t glowGainIn, float32_t glowMid) {
		if (ycIn <= 2.0 / 3.0 * glowMid) {
			return glowGainIn;

		} else if (ycIn >= 2.0 * glowMid) {
			return 0.0;
		}

		return glowGainIn * (glowMid / ycIn - 1.0 / 2.0);
	}

	//! @reference [glow_inv(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.RRT_Common.ctl#L44-L57)
	float32_t GlowInv(float32_t ycOut, float32_t glowGainIn, float32_t glowMid) {
		if (ycOut <= ((1.0 + glowGainIn) * 2.0 / 3.0 * glowMid)) {
			return -glowGainIn / (1.0 + glowGainIn);

		} else if (ycOut >= (2.0 * glowMid)) {
			return 0.0;
		}

		return glowGainIn * (glowMid / ycOut - 1.0 / 2.0) / (glowGainIn / 2.0 - 1.0);
	}

	//! @reference [sigmoid_shaper(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.RRT_Common.ctl#L59-L67)
	float32_t SigmoidShaper(float32_t x) {
		// Sigmoid function in the range 0 to 1 spanning -2 to +2.

		const float32_t t = max(1.0 - abs(x / 2.0), 0.0);
		const float32_t y = 1.0 + sign(x) * (1.0 - t * t);
		return y / 2.0;
	}

	// --- Red modifier --- //

	//! @reference [cubic_basis_shaper(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.RRT_Common.ctl#L71-L115)
	float32_t CubicBasisShaper(float32_t x, float32_t w) {

		const float32_t M[4][4] = {
			{ -1.0 / 6.0,  3.0 / 6.0, -3.0 / 6.0,  1.0 / 6.0 },
			{  3.0 / 6.0, -6.0 / 6.0,  3.0 / 6.0,  0.0 / 6.0 },
			{ -3.0 / 6.0,  0.0 / 6.0,  3.0 / 6.0,  0.0 / 6.0 },
			{  1.0 / 6.0,  4.0 / 6.0,  1.0 / 6.0,  0.0 / 6.0 }
		};

		const float32_t knots[5] = { -w * 0.5, -w * 0.25, 0.0, w * 0.25, w * 0.5 };

		float32_t y = 0.0;
		if ((x > knots[0]) && (x < knots[4])) {
			const float32_t knot_coord = (x - knots[0]) * 4.0 / w;
			const int32_t j            = (int32_t)knot_coord;
			const float32_t t          = knot_coord - j;

			const float32_t monomials[4] = { t * t * t, t * t, t, 1.0 };

			// (if/else structure required for compatibility with CTL < v1.5.)
			if (j == 3) {
				y = monomials[0] * M[0][0] + monomials[1] * M[1][0] + monomials[2] * M[2][0] + monomials[3] * M[3][0];

			} else if (j == 2) {
				y = monomials[0] * M[0][1] + monomials[1] * M[1][1] + monomials[2] * M[2][1] + monomials[3] * M[3][1];

			} else if (j == 1) {
				y = monomials[0] * M[0][2] + monomials[1] * M[1][2] + monomials[2] * M[2][2] + monomials[3] * M[3][2];

			} else if (j == 0) {
				y = monomials[0] * M[0][3] + monomials[1] * M[1][3] + monomials[2] * M[2][3] + monomials[3] * M[3][3];
			}
		}

		return y * 3.0 / 2.0;
	}

	//! @reference [center_hue(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.RRT_Common.ctl#L117-L123)
	float32_t CenterHue(float32_t hue, float32_t centerH) {
		float32_t hueCentered = hue - centerH;
		if (hueCentered < -180.0) {
			hueCentered = hueCentered + 360.0;

		} else if (hueCentered > 180.0) {
			hueCentered = hueCentered - 360.0;
		}

		return hueCentered;
	}

	//! @reference [uncenter_hue(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.RRT_Common.ctl#L125-L131)
	float32_t UncenterHue(float32_t hueCentered, float32_t centerH) {
		float32_t hue = hueCentered + centerH;
		if (hue < 0.0) {
			hue = hue + 360.0;

		} else if (hue > 360.0) {
			hue = hue - 360.0;
		}

		return hue;
	}


	//! @reference [rrt_sweeteners(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.RRT_Common.ctl#L135-L162)
	float32_t3 RRTSweeteners(float32_t3 aces_in) {

		float32_t3 aces = aces_in;

		// --- Glow module --- //
		const float32_t saturation = rgb2saturation(aces);
		const float32_t ycIn       = rgb2yc(aces);
		const float32_t s          = SigmoidShaper((saturation - 0.4) / 0.2);
		const float32_t addedGlow  = 1.0 + GlowFwd(ycIn, RRT_GLOW_GAIN * s, RRT_GLOW_MID);

		aces = addedGlow * aces;

		// --- Red modifier --- //
		const float32_t hue         = rgb2hue(aces);
		const float32_t centeredHue = CenterHue(hue, RRT_RED_HUE);
		const float32_t hueWeight   = CubicBasisShaper(centeredHue, RRT_RED_WIDTH);

		aces.x = aces.x + hueWeight * saturation * (RRT_RED_PIVOT - aces.x) * (1.0 - RRT_RED_SCALE);

		// --- ACES to RGB rendering space --- //
		aces = max(aces, 0.0); // clamp_f3(aces, 0., HALF_POS_INF). avoids saturated negative colors from becoming positive in the matrix
		float32_t3 rgbPre = mul(aces, AP0_2_AP1_MAT);
		rgbPre = clamp(rgbPre, 0.0, ACES_HALF_MAX); // clamp_f3(rgbPre, 0., HALF_POS_INF). avoids saturated negative colors from becoming positive in the matrix

		// --- Global desaturation --- //
		rgbPre = mul(rgbPre, RRT_SAT_MAT);

		return rgbPre;
	}

}

#endif
