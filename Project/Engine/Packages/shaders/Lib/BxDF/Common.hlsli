#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// BxDF namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace BxDF {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Context structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Context {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float32_t NdotV;
		float32_t NdotL;
		float32_t NdotH;
		float32_t VdotH;

		//=========================================================================================
		// public methods
		//=========================================================================================

		bool IsValid() {
			return NdotL > 0.0;
		}

		static Context Create(float32_t3 n, float32_t3 v, float32_t3 l) {

			Context context;

			float32_t3 h = normalize(l + v); //!< half vector

			context.NdotV = saturate(dot(n, v));
			context.NdotL = saturate(dot(n, l));
			context.NdotH = saturate(dot(n, h));
			context.VdotH = saturate(dot(v, h));

			return context;
			// TODO: Epsilon対策
		}

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Reflectance structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Reflectance {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float32_t3 diffuse;
		float32_t3 specular;

		//=========================================================================================
		// public methods
		//=========================================================================================

		static Reflectance Create(float32_t3 albedo, float32_t metallic) {

			Reflectance reflectance;

			static const float32_t3 kMinFresnel = 0.04; //!< 非金属の最小Fresnel値

			reflectance.diffuse  = albedo * (1.0 - kMinFresnel) * (1.0 - metallic);
			reflectance.specular = lerp(kMinFresnel, albedo, metallic);

			return reflectance;
		}

	};

}
