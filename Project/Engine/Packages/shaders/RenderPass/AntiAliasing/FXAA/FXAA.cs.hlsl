//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../AntiAliasing.hlsli"

//* fxaa
#define FXAA_QUALITY__PRESET 39
#define FXAA_DISCARD 0
#include "FXAAQuality.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//!< @brief Input情報
Texture2D<float32_t4> gInput : register(t0);

//!< @brief Output情報
RWTexture2D<float32_t4> gOutput : register(u0);

//!< @brief Sampler情報
SamplerState gSamplerLinearClamp : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("compute")]
[numthreads(8, 8, 1)]
void main(Semantics::Compute semantics) {

	uint32_t2 pixel = semantics.dispatch_thread_id.xy;

	if (!Context.IsWithinRenderResolution(pixel)) {
		return; //!< 画面外のpixelは処理しない
	}

	float32_t2 texcoord = (float32_t2(pixel) + 0.5) / float32_t2(Context.render_resolution);

	FXAA::Tex t;
	t.sampler_linear_clamp = gSamplerLinearClamp;
	t.tex                  = gInput;
	t.min_uv               = float32_t2(0, 0);
	t.max_uv               = float32_t2(1, 1);

	//!< FXAAのパラメータの設定
	float32_t subpix             = 0.5;
	float32_t edge_threshold     = 0.125;
	float32_t edge_threshold_min = 0.0625;

	float32_t4 color = FXAA::FXAAQuality(
		texcoord, t, 1.0 / float32_t2(Context.render_resolution),
		subpix, edge_threshold, edge_threshold_min
	);

	gOutput[pixel] = color;
}
