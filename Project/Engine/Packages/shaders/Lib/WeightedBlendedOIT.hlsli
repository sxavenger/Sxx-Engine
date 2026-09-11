#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Numeric.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Weighted-Blended-OIT namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace WeightedBlendedOIT {

	// # reference
	// [Weighted Blended Order-Independent Transparency](https://jcgt.org/published/0002/02/09/paper.pdf)
	// [OpenGL / Weighted Blended](https://learnopengl.com/Guest-Articles/2020/OIT/Weighted-Blended)

	//=========================================================================================
	// methods
	//=========================================================================================

	//! @brief Calculate weight for Weighted Blended OIT
	//! @param[in] transparency: fragment transparency
	//! @param[in] depth: fragment depth (SV_Position.z)
	//! @param[in] near: camera near plane
	//! @param[in] far: camera far plane
	float32_t CalculateWeight(float32_t transparency, float32_t depth, float32_t near, float32_t far) {
		//!< [Weighted Blended Order-Independent Transparency] Depth Weights Improve Occlusion (Eq.10)
		float32_t d = ((near * far) / -depth - far) / (near - far);

		float32_t w = max(1e-2, 3e3 * pow(1.0f - d, 3.0f));
		return transparency * w;
	}

	//! @brief Check if the value is approximately opaque (1.0)
	//! @param[in] v: value to check
	bool IsApproximatelyOpacity(float32_t v) {
		return abs(v - 1.0f) <= max(abs(v), 1.0f) * Numeric::kEpsilon;
	}

	//! @brief Calculate the final transparent color from accumulation buffer
	//! @param[in] accumulation: accumulation buffer value
	float32_t3 CalculateTransparentColor(float32_t4 accumulation) {
		return accumulation.rgb / max(accumulation.a, Numeric::kEpsilon);
	}

}
