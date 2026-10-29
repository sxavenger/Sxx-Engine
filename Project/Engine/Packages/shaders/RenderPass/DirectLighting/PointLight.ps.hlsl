//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Lighting.hlsli"

//* component
#include "../../Component/Light/PointLight.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//! @brief PointLight情報
ConstantBuffer<Component::PointLight> gPointLight    : register(b0);
ConstantBuffer<Component::Transform> gLightTransform : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PixelOutputData main(PixelRasterData input) {

	//!< GBufferの情報を取得する
	GBuffer::Reconstructor reconstructor
		= GBuffer::Reconstructor::Create(uint32_t2(input.position.xy), Context.resolution, gCameraProjection.projection_inverse, gCameraTransform.world);

	const GBuffer::Surface surface = reconstructor.RecnstructSurface(kTextures);

	if (!surface.HasGeometry()) {
		discard; //!< surfaceが存在しない場合は描画しない
	}

	//!< light情報の取得
	float32_t3 l
		= Component::PointLight::GetDirectionFromSurface(gLightTransform.GetPosition(), surface.position); //!< surfaceからlightへの方向ベクトル

	//!< camera情報の取得
	float32_t3 v
		= normalize(gCameraTransform.GetPosition() - surface.position); //!< surfaceからcameraへの方向ベクトルを取得

	//!< BxDF情報の初期化
	BxDF::Context context         = BxDF::Context::Create(surface.normal, v, l);
	BxDF::Reflectance reflectance = BxDF::Reflectance::Create(surface.albedo, surface.metallic);

	if (!context.IsValid()) {
		discard; //!< Contextが不正な場合は描画しない
	}

	//!< 光源の影響範囲を取得する
	float32_t3 color_mask = gPointLight.GetColorMask();
	float32_t light_mask  = gPointLight.GetLightMask(gScene, l, surface.position);

	//!< BRDFの評価
	float32_t3 lo = BRDF::Evaluate(context, reflectance, surface.roughness) * context.NdotL * color_mask * light_mask * surface.ambient_occlusion;

	PixelOutputData output = (PixelOutputData)0;
	output.buffer0.rgb = lo;
	output.buffer0.a   = 1.0f;

	return output;

}
