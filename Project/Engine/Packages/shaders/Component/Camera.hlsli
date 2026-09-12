#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// Component namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Component {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Camera structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Camera {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float32_t4x4 projection;
		float32_t4x4 projection_inverse;
		float32_t near;
		float32_t far;

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* camera options *//

		float32_t4x4 GetProjection() {
			return projection;
		}

		//* matrix methods *//

		static float32_t4x4 MakePerspective(float32_t fov, float32_t aspect, float32_t near, float32_t far) {
			return float32_t4x4(
				1.0 / (aspect * tan(fov * 0.5)), 0.0, 0.0, 0.0,
				0.0, 1.0 / tan(fov * 0.5), 0.0, 0.0,
				0.0, 0.0, far / (far - near), 1.0,
				0.0, 0.0, -near * far / (far - near), 0.0
			);
		}

		static float32_t4x4 MakeOrthographic(float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t near, float32_t far) {
			return float32_t4x4(
				2.0 / (right - left), 0.0, 0.0, 0.0,
				0.0, 2.0 / (top - bottom), 0.0, 0.0,
				0.0, 0.0, 1.0 / (far - near), 1.0,
				-(right + left) / (right - left), -(top + bottom) / (top - bottom), -near / (far - near), 1.0
			);
		}

		static float32_t4x4 MakeViewport(float32_t x, float32_t y, float32_t width, float32_t height, float32_t near, float32_t far) {
			return float32_t4x4(
				width * 0.5, 0.0, 0.0, 0.0,
				0.0, -height * 0.5, 0.0, 0.0,
				0.0, 0.0, far - near, 1.0,
				x + width * 0.5, y + height * 0.5, near, 1.0
			);
		}

	};

}


