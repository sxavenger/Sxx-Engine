//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Lighting.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("pixel")]
[earlydepthstencil]
PixelOutputData main(PixelRasterData input) {

	//!< GBufferの情報を取得する
	GBuffer::Reconstructor reconstructor
		= GBuffer::Reconstructor::Create(uint32_t2(input.position.xy), Context.resolution, gCameraProjection.projection_inverse, gCameraTransform.world);

	const GBuffer::Surface surface = reconstructor.RecnstructSurface(kTextures);

	if (!surface.HasGeometry()) {
		discard; //!< surfaceが存在しない場合は描画しない
	}

	PixelOutputData output = (PixelOutputData)0;
	output.buffer0.rgb = 0.0;
	output.buffer0.a   = 1.0;

	return output;

}
