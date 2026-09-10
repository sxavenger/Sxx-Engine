//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Simple.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
FragmentOutputData main(FragmentInputData input) {

	FragmentOutputData output;

	output.color = float32_t4(input.normal * 0.5f + 0.5f, 1.0f); //!< 法線を表示
	// output.color = float32_t4(input.texcoord, 0.0f, 1.0f); //!< texcoordを表示
	// output.color = float32_t4(input.raster.position.z, 0.0f, 0.0f, 1.0f); //!< depthを表示

	return output;

}
