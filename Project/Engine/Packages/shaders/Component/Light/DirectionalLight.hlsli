#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightEnum.hlsli"

//* lib
#include "../../Lib/Mathmatic.hlsli"
#include "../../Lib/Flag.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Component namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Component {

	////////////////////////////////////////////////////////////////////////////////////////////
	// DirectionalLight structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct DirectionalLight {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float32_t3 light_color;
		float32_t light_intensity; //!< [lux]

		uint32_t shadow_flags;
		float32_t shadow_strength; //!< [0.0 - 1.0]

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief surfaceからの光源方向を取得する
		static float32_t3 GetDirectionFromSurface(float32_t3 light_direction) {
			return -light_direction;
		}

		//! @brief surfaceから光源方向に向かってレイを飛ばし、影の減衰値を取得する
		float32_t CalculateShadowAttenuation(
			RaytracingAccelerationStructure scene,
			float32_t3 light_direction,
			float32_t3 surface_position) {

			FlagEnum<ShadowFlags> flags = FlagEnum<ShadowFlags>(shadow_flags);

			//!< Surfaceから光源方向に向かってレイを飛ばす
			bool hit = TraceShadow(
				scene,
				light_direction,
				10000.0,
				surface_position,
				flags
			);

			return hit ? 1.0 - shadow_strength : 1.0;
		}

		//! @brief 影の減衰値を取得する
		float32_t GetLightMask(RaytracingAccelerationStructure scene, float32_t3 light_direction, float32_t3 surface_position) {

			float32_t attenuation_shadow = CalculateShadowAttenuation(scene, light_direction, surface_position);

			return attenuation_shadow;
		}

		//! @brief 色の減衰値を取得する
		float32_t3 GetColorMask() {
			return light_color * light_intensity;
		}

	};

}
