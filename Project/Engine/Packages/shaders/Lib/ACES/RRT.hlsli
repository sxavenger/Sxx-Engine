#pragma once

// # reference
// [RRT.ctl](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/rrt/RRT.ctl)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Utilities.hlsli"
#include "TransformCommon.hlsli"
#include "RRTCommon.hlsli"
#include "Tonescales.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// ACES namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ACES {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief RRT main function | ACES2065-1 (AP0) -> OCES (AP0)
	//! @param aces ACES
	//! @return OCES
	//! @reference [<RRT> main(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/rrt/RRT.ctl#L21-L75)
	float32_t3 RRTMain(float32_t3 aces) {

		float32_t3 rgbPre = RRTSweeteners(aces);
		//!< Glow module 〜 Global desaturation の処理は rrt_sweeteners() と同一なのでそれを呼んでいる.

		// --- Apply the tonescale independently in rendering-space RGB --- //

		float32_t3 rgbPost = SegmentedSplineC5Fwd(rgbPre, RRT_PARAMS);

		// --- RGB rendering space to OCES --- //

		return mul(rgbPost, AP1_2_AP0_MAT);
	}

}
