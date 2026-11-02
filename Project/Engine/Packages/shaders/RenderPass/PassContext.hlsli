#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include "../Lib/ACES/ColorSpace.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// DisplayColorSpace enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class DisplayColorSpace : uint32_t { //!< [Graphics::DisplayColorSpace 参照]
	Rec709 = 0,
};

////////////////////////////////////////////////////////////////////////////////////////////
// WorkingColorSpace enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class WorkingColorSpace : uint32_t { //!< [Rendering::PassOptions::WorkingColorSpace 参照]
	Linear = 0,
	ACEScg = 1,
};

////////////////////////////////////////////////////////////////////////////////////////////
// DisplayEncoder enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class DisplayEncoder : uint32_t { //!< [Rendering::PassOptions::DisplayEncoder 参照]
	Software = 0,
	Hardware = 1,
};

////////////////////////////////////////////////////////////////////////////////////////////
// PassContext structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PassContext {

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint32_t2 output_resolution;
	uint32_t2 render_resolution;
	WorkingColorSpace working_color_space;
	DisplayColorSpace display_color_space;
	DisplayEncoder display_encoder;

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief インデックスが出力解像度内に収まっているかを判定
	bool IsWithinOutputResolution(uint32_t2 index) {
		return all(index < output_resolution);
	}

	//! @brief インデックスがレンダリング解像度内に収まっているかを判定
	bool IsWithinRenderResolution(uint32_t2 index) {
		return all(index < render_resolution);
	}

	//! @brief 入力色をWorkingColorSpaceに変換
	float32_t3 InputColorSpace(float32_t3 input) {
		[branch]
		switch (working_color_space) {
			case WorkingColorSpace::ACEScg:
				return ACES::Rec709ToAP1(input); //!< Rec.709 -> ACEScg

			case WorkingColorSpace::Linear:
			default:
				return input; //!< Linear
		}
	}

};
