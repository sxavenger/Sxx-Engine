#pragma once

// # Light単位について
// ## Directional Light
// luxで表現
// ## Point Light - Spot Light
// cdで表現 (lmはcache時点で変換される) [cd] = [lm/sr] = [lm/m^2] * [m^2/sr]

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include "../../Lib/Flag.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// ShadowFlags enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class ShadowFlags : uint32_t { //!< [ShadowFlags.h参照]
	CastShadow   = 1 << 0,
	CullBackFace = 1 << 1,
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

bool TraceShadow(
	RaytracingAccelerationStructure scene,
	float3 light_direction, float32_t light_distance,
	float3 surface_position, FlagEnum<ShadowFlags> shadow_flags) {

	if (!shadow_flags.Test(ShadowFlags::CastShadow)) {
		return false;
	}

	//!< Surfaceから光源方向に向かってレイを飛ばす
	RayDesc desc;
	desc.Origin    = surface_position;
	desc.Direction = light_direction;
	desc.TMin      = 0.02;
	desc.TMax	   = light_distance;

	//!< レイのフラグを設定する
	static const uint32_t ray_flags
		= RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH
		| RAY_FLAG_FORCE_OPAQUE
		| RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES;

	//!< CullBackFaceが有効な場合は裏面をカリングする
	uint32_t trace_flag = shadow_flags.Test(ShadowFlags::CullBackFace) ? RAY_FLAG_CULL_BACK_FACING_TRIANGLES : RAY_FLAG_NONE;

	RayQuery<ray_flags> query;
	query.TraceRayInline(
		scene,
		trace_flag,
		0b01,
		desc
	);

	query.Proceed(); //!< 不透明判定のみ1回で確定
	bool hit = query.CommittedStatus() == COMMITTED_TRIANGLE_HIT;

	return hit;
}
