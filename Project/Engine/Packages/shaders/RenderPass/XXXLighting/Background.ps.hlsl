
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

	float32_t3 direction = gCameraTransform.GetForward(); //!< カメラの前方向を取得する

	float32_t3 environment_color = gSkyLight.GetEnvironment(direction, gLinearWarpSampler); //!< SkyLightの環境光を取得する

	PixelOutputData output = (PixelOutputData)0;
	output.buffer0.rgb = environment_color;
	output.buffer0.a   = 1.0;

	return output;

}
