#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightEnum.hlsli"

//* lib
#include "../../Lib/Flag.hlsli"
#include "../../Lib/Mathmatic.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Component namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Component {

	////////////////////////////////////////////////////////////////////////////////////////////
	// PointLight structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PointLight {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float32_t3 light_color;
		float32_t light_intensity; //!< [cd/m^2]
		float32_t light_radius;

		uint32_t shadow_flags;
		float32_t shadow_strength; //!< [0.0 - 1.0]

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief surfaceからの光源方向を取得する
		static float32_t3 GetDirectionFromSurface(float32_t3 light_position, float32_t3 surface_position) {
			return normalize(light_position - surface_position);
		}

		//! @brief surfaceから光源方向に向かってレイを飛ばし、影の減衰値を取得する
		float32_t CalculateShadowAttenuation(
			RaytracingAccelerationStructure scene,
			float32_t3 light_direction, float32_t light_distance,
			float32_t3 surface_position) {

			FlagEnum<ShadowFlags> flags = FlagEnum<ShadowFlags>(shadow_flags);

			//!< Surfaceから光源方向に向かってレイを飛ばす
			bool hit = TraceShadow(
				scene,
				light_direction,
				light_distance,
				surface_position,
				flags
			);

			float32_t attenuation_shadow = 1.0;

			if (hit) {
				attenuation_shadow = 1.0 - shadow_strength; //!< 影がある場合は減衰値を計算する
			}

			return attenuation_shadow;
		}

		//! @brief 距離減衰値を取得する
		float32_t CalculateRadiusAttenuation(
			float32_t light_distance) {

			float32_t distance_sq = Mathmatic::Square(light_distance);
			float32_t factor      = distance_sq / Mathmatic::Square(light_radius); //!< (d/r)^2

			float32_t window = Mathmatic::Square(saturate(1.0f - Mathmatic::Square(factor))); //!< saturate(1 - (d/r)^4)^2

			float32_t attenuation_radius = window / max(distance_sq, Mathmatic::Square(0.01f));

			return attenuation_radius;
		}

		//! @brief 影の減衰値を取得する
		float32_t GetLightMask(
			RaytracingAccelerationStructure scene,
			float32_t3 light_position, float32_t3 surface_position) {

			float32_t light_distance   = length(light_position - surface_position);
			float32_t3 light_direction = GetDirectionFromSurface(light_position, surface_position);

			float32_t attenuation_radius = CalculateRadiusAttenuation(light_distance);
			float32_t attenuation_shadow = CalculateShadowAttenuation(scene, light_direction, light_distance, surface_position);

			return attenuation_radius * attenuation_shadow;
		}

		//! @brief 色の減衰値を取得する
		float32_t3 GetColorMask() {
			return light_color * light_intensity;
		}

	};

}
