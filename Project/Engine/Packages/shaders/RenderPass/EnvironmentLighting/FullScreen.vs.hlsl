//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Lighting.hlsli"

//-----------------------------------------------------------------------------------------
// constant variables
//-----------------------------------------------------------------------------------------

static const float32_t2 kPositions[3] = {
	float32_t2(-1.0,  1.0),
	float32_t2( 3.0,  1.0),
	float32_t2(-1.0, -3.0)
};

static const float32_t kFarDepth = 1.0;
//!< 手前側のobjectのみにLightingを適用するため、FarDepthを1.0に設定する

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("vertex")]
PixelRasterData main(Semantics::Vertex semantics) {

	PixelRasterData output = (PixelRasterData)0;

	output.position    = float32_t4(kPositions[semantics.vertex_id], kFarDepth, 1.0);
	output.instance_id = semantics.instance_id;

	return output;

}
