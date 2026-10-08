//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Simple.hlsli"

//* Dev
#include "../../Dev/Visualization.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PixelOutputData main(PixelInputData input, PixelSemantics semantics) {

	PixelOutputData output = (PixelOutputData)0;

	output.color.rgb = Dev::IntToColor(semantics.primitive_id);
	output.color.a   = 1.0f;

	return output;

}
