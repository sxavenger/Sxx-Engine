#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightEnum.hlsli"

//* lib
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
		float32_t3 GetDirectionFromSurface(float32_t3 light_direction) {
			return -light_direction;
		}

		//! @brief surfaceから光源方向に向かってレイを飛ばし、影の減衰値を取得する
		float32_t TraceShadowAttenuation(RaytracingAccelerationStructure scene, float32_t3 light_direction, float32_t3 surface_position) {

			FlagEnum<ShadowFlags> flags = FlagEnum<ShadowFlags>(shadow_flags);

			if (!flags.Test(ShadowFlags::CastShadow)) {
				return 1.0f;
			}

			//!< Surfaceから光源方向に向かってレイを飛ばす
			RayDesc desc;
			desc.Origin    = surface_position;
			desc.Direction = light_direction;
			desc.TMin      = 0.02f;
			desc.TMax	   = 10000.0f;

			//!< レイのフラグを設定する
			static const uint32_t ray_flags
				= RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH
				| RAY_FLAG_FORCE_OPAQUE
				| RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES;

			//!< CullBackFaceが有効な場合は裏面をカリングする
			uint32_t trace_flag = flags.Test(ShadowFlags::CullBackFace) ? RAY_FLAG_CULL_BACK_FACING_TRIANGLES : RAY_FLAG_NONE;

			RayQuery<ray_flags> query;
			query.TraceRayInline(
				scene,
				trace_flag,
				0b01,
				desc
			);

			query.Proceed(); //!< 不透明判定のみ1回で確定
			bool hit = query.CommittedStatus() == COMMITTED_TRIANGLE_HIT;

			return hit ? 1.0f - shadow_strength : 1.0f;
		}

		float32_t GetLightMask(RaytracingAccelerationStructure scene, float32_t3 light_direction, float32_t3 surface_position) {
			float32_t attenuation_shadow = TraceShadowAttenuation(scene, light_direction, surface_position);
			return attenuation_shadow;
		}

		float32_t3 GetColorMask() {
			return light_color * light_intensity;
		}

	};

}
