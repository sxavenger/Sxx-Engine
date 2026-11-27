//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Deferred.hlsli"

//* mateiral
#include "../../Material/SurfaceMaterial.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

StructuredBuffer<SurfaceMaterial> gMaterial : register(t0); //!< HACK: 構造体のレイアウトを修正してConstantBufferにする
SamplerState gLinearWarpSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("pixel")]
[earlydepthstencil]
PixelOutputData main(PixelInputData input) {

	float32_t transparency = gMaterial[0].transparency.Get(gLinearWarpSampler, input.texcoord);
	if (transparency < alpha_cutoff) {
		discard; //!< 半透明描画を破棄
	}

	float32_t3 albedo   = gMaterial[0].albedo.Get(gLinearWarpSampler, input.texcoord);
	float32_t3 normal   = gMaterial[0].normal.Get(normalize(input.normal), normalize(input.tangent), normalize(input.bitangent), gLinearWarpSampler, input.texcoord);
	float32_t roughness = gMaterial[0].roughness.Get(gLinearWarpSampler, input.texcoord);
	float32_t metallic  = gMaterial[0].metallic.Get(gLinearWarpSampler, input.texcoord);

	albedo = Context.InputColorSpace(albedo); //!< albedoを入力色空間に変換する

	//!< GBufferに書き込み
	PixelOutputData output = (PixelOutputData)0;

	output.SetAlbedo(albedo);
	output.SetNormal(normal);
	output.SetRoughness(roughness);
	output.SetMetallic(metallic);
	output.SetAmbientOcclusion(1.0); //!< TODO: AmbientOcclusionの計算を追加する

	return output;

}
