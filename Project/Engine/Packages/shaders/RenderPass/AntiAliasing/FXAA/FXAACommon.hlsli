#pragma once

// # reference
// [Fxaa_3.11.hlsli](https://gist.github.com/jwoolston/c96c22473110c9924d831cebb5f2cdb4)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "FXAADefine.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// FXAA namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace FXAA {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Tex structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Tex {

		//=========================================================================================
		// public variables
		//=========================================================================================

		SamplerState sampler_linear_clamp;
		Texture2D<Float4> tex;
		Float2 min_uv;
		Float2 max_uv;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	Float4 TexTop(Tex t, Float2 texcoord) {
		return t.tex.SampleLevel(t.sampler_linear_clamp, clamp(texcoord, t.min_uv, t.max_uv), 0);
	}

	Float4 TexTopOff(Tex t, Float2 texcoord, Int2 offset) {
		return t.tex.SampleLevel(t.sampler_linear_clamp, clamp(texcoord, t.min_uv, t.max_uv), 0, offset);
	}

	//! @brief RGBから輝度を計算する.
	Float Luma(Float3 rgb) {
		return dot(rgb, Float3(0.2722287168f, 0.6740817658f, 0.0536895174f));
	}

};
