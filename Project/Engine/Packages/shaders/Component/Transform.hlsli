#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// Component namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Component {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transform structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Transform {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float32_t4x4 world;
		float32_t4x4 world_inverse;

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* transform option *//

		float32_t4 TransformPoint(float32_t4 position) {
			return mul(position, world);
		}

		float32_t3 TransformPoint(float32_t3 position) {
			return mul(float32_t4(position, 1.0f), world).xyz;
		}

		float32_t3 TransformNormal(float32_t3 normal) {
			return mul(normal, (float32_t3x3)transpose(world_inverse)).xyz;
			//!< 法線の変換のため, world_inverse の転置行列を使用する.
		}

		float32_t4x4 GetView() {
			return world_inverse;
		}

		float32_t3 GetPosition() {
			return world[3].xyz;
		}

	};

}
