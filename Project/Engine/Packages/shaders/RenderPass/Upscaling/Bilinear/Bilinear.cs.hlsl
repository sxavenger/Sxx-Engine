//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../UpscalingCommon.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//!< @brief Render情報
Texture2D<float32_t4> gSceneRender : register(t0);

//!< @brief Output情報
RWTexture2D<float32_t4> gSceneOutput : register(u0);

//!< @brief Sampler情報
SamplerState gSamplerLinearClamp : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("compute")]
[numthreads(8, 8, 1)] //!< output_resolutionでdispatchする.
void main(Semantics::Compute semantics) {

	uint32_t2 pixel = semantics.dispatch_thread_id.xy;

	if (!Context.IsWithinOutputResolution(pixel)) {
		return; //!< 画面外のpixelは処理しない
	}

	float32_t2 texcoord = (float32_t2(pixel) + 0.5) / float32_t2(Context.output_resolution);

	gSceneOutput[pixel] = gSceneRender.SampleLevel(gSamplerLinearClamp, texcoord, 0);
}
