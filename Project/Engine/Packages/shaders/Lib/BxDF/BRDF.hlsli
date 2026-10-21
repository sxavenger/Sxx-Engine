#pragma once

// # reference
// [Real Shading in Unreal Engine 4](https://de45xmedrsdbp.cloudfront.net/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf)
// [Physically Based Shading on Mobile](https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile)
// [Filament: Physically Based Rendering](https://google.github.io/filament/Filament.md.html#materialsystem/diffusebrdf)
// [Height-Correlated Smith Function](https://light11.hatenadiary.com/entry/2020/03/03/195249#Height-Correlated-Smith%E9%96%A2%E6%95%B0)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Common.hlsli"
#include "../Numeric.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// BRDF namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace BRDF {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//* --- Diffuse BRDF --- *//

	//! @brief Lambert diffuse BRDF
	//! @param albedo diffuse color (BxDF::Reflectance::diffuse)
	float32_t3 Fd_Lambert(float32_t3 albedo) {
		return albedo * (1.0f / Numeric::kPi);
	}

	//* --- Specular BRDF --- *//

	//! @brief Fresnel term (Schlick, Spherical Gaussian approximation) [Karis 2013]
	float32_t3 F_SchlickSG(BxDF::Context context, BxDF::Reflectance reflectance) {
		float32_t exponent = (-5.55473f * context.VdotH - 6.98316f) * context.VdotH;
		return reflectance.specular + (1.0f - reflectance.specular) * exp2(exponent);
	}

	//! @brief Geometric shadowing term (Smith-Schlick-GGX, k = (roughness + 1)^2 / 8) [Karis 2013]
	//! @param roughness perceptual roughness
	float32_t G_SmithSchlickGGX(BxDF::Context context, float32_t roughness) {
		const float32_t a = roughness + 1.0f;
		const float32_t k = (a * a) / 8.0f;

		float32_t G1_V = context.NdotV / (context.NdotV * (1.0f - k) + k);
		float32_t G1_L = context.NdotL / (context.NdotL * (1.0f - k) + k);

		return G1_V * G1_L;
	}

	//! @brief Visibility term (height-correlated Smith GGX) [Heitz 2014]
	//! @param roughness perceptual roughness
	float32_t V_SmithGGXCorrelated(BxDF::Context context, float32_t roughness) {
		float32_t alpha = roughness * roughness;
		float32_t a2    = alpha * alpha;

		float32_t GGXV = context.NdotL * sqrt(context.NdotV * context.NdotV * (1.0f - a2) + a2);
		float32_t GGXL = context.NdotV * sqrt(context.NdotL * context.NdotL * (1.0f - a2) + a2);
		return 0.5f / max(GGXV + GGXL, Numeric::kEpsilon);
	}

	//! @brief Normal distribution function (GGX / Trowbridge-Reitz)
	//! @param roughness perceptual roughness
	float32_t D_GGX(BxDF::Context context, float32_t roughness) {
		float32_t alpha = roughness * roughness;
		float32_t a2    = alpha * alpha;

		float32_t f = context.NdotH * context.NdotH * (a2 - 1.0f) + 1.0f;
		return a2 / (Numeric::kPi * f * f);
	}

	// note
	// g項を使う場合 specular = F * G * D / (4 * NdotL * NdotV)
	// v項を使う場合 specular = F * V * D

	//! @brief Cook-Torrance microfacet BRDF
	float32_t3 Evaluate(BxDF::Context context, BxDF::Reflectance reflectance, float32_t roughness) {
		float32_t3 fd = Fd_Lambert(reflectance.diffuse);
		float32_t3 fs = F_SchlickSG(context, reflectance) * V_SmithGGXCorrelated(context, roughness) * D_GGX(context, roughness);

		return fd + fs;
	}

}
