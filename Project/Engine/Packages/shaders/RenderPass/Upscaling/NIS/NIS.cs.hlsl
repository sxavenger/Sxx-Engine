//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../UpscalingCommon.hlsli"

#define NIS_SCALER 1
#define NIS_HDR_MODE 1 //!< (NIS_HDR_MODE_LINEAR)
#define NIS_USE_HALF_PRECISION 1
#include "NISScaler.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//!< @brief Render情報
Texture2D<float32_t4> gSceneRender : register(t0);

//!< @brief Output情報
RWTexture2D<float32_t4> gSceneOutput : register(u0);

//!< @brief Sampler情報
SamplerState gSamplerLinearClamp : register(s0);

//!< @brief Coefficient情報
Texture2D<float32_t4> gCoefScaler : register(t1);
Texture2D<float32_t4> gCoefUsm    : register(t2);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("compute")]
[numthreads(NIS_THREAD_GROUP_SIZE, 1, 1)]
void main(Semantics::Compute semantics) {

	NIS::Config config = NIS::Config::NVScalerUpdateConfig(
		sharpness,
		float32_t2(0, 0), Context.render_resolution, Context.render_resolution,
		float32_t2(0, 0), Context.output_resolution, Context.output_resolution,
		NIS::HDRMode::Linear
	);

	NIS::Bindings bindings;
	bindings.in_texture           = gSceneRender;
	bindings.out_texture          = gSceneOutput;
	bindings.sampler_linear_clamp = gSamplerLinearClamp;
	bindings.coef_scaler          = gCoefScaler;
	bindings.coef_usm             = gCoefUsm;

#if NIS_SCALER
	NIS::NVScaler(config, bindings, semantics.group_id.xy, semantics.group_thread_id.x);
#else
	NIS::NVSharpen(config, bindings, semantics.group_id.xy, semantics.group_thread_id.x);
#endif
}
