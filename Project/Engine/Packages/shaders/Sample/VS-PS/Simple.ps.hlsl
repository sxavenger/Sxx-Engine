//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Simple.hlsli"

//* Dev
#include "../../Dev/Visualization.hlsli"

//* mateiral
#include "../../Material/SurfaceMaterial.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

ConstantBuffer<SurfaceMaterial> gMaterial : register(b0);
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PixelOutputData main(PixelInputData input, PixelSemantics semantics) {

	PixelOutputData output = (PixelOutputData)0;

	output.color.rgb = Dev::IntToColor(semantics.primitive_id);
	output.color.a   = 1.0f;

	output.color.rgb = gMaterial.albedo.Get(gSampler, input.texcoord);

	return output;

}
