
//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Lighting.hlsli"

//* component
#include "../../Component/Light/SkyLight.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//! @brief SkyLight情報
ConstantBuffer<Component::SkyLight> gSkyLight : register(b0);

//! @brief sampler情報
SamplerState gLinearWarpSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("pixel")]
[earlydepthstencil]
PixelOutputData main(PixelRasterData input) {

	//!< GBufferの情報を取得する
	GBuffer::Reconstructor reconstructor
		= GBuffer::Reconstructor::Create(uint32_t2(input.position.xy), Context.render_resolution, gCameraProjection.projection_inverse, gCameraTransform.world);

	const GBuffer::Surface surface = reconstructor.RecnstructSurface(kTextures);

	if (surface.HasGeometry()) {
		discard; //!< backgroundの描画は、surfaceが存在しない場合のみ行う
	}

	//!< カメラの前方向を取得する
	float32_t3 view_position = gCameraProjection.ReconstructViewPosition(uint32_t2(input.position.xy), Context.render_resolution, 1.0);
	float32_t3 direction     = gCameraTransform.TransformNormal(normalize(view_position));

	float32_t3 environment_color = gSkyLight.GetEnvironment(direction, gLinearWarpSampler); //!< 環境光の取得
	float32_t3 color_mask        = gSkyLight.GetColorMask();

	float32_t3 color = ACES::Rec709ToAP1(environment_color) * color_mask; //!< Rec709 -> ACEScg

	PixelOutputData output = (PixelOutputData)0;
	output.buffer0.rgb = color;
	output.buffer0.a   = 1.0;

	return output;

}
