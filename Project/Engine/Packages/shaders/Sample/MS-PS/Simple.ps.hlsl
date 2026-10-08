//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Simple.hlsli"

//* Dev
#include "../../Dev/Visualization.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PixelOutputData main(PixelInputData input) {

	PixelOutputData output = (PixelOutputData)0;

	output.color.rgb = Dev::IntToColor(input.raster.meshlet_index);
	output.color.a   = 1.0f;

	return output;

}
