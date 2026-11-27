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

		//! @reference https://mynameismjp.wordpress.com/2010/09/05/position-from-depth-3/
		float32_t LinearDepth(float32_t ndc_depth) {
			float32_t projection_a = far / (far - near);
			float32_t projection_b = (-far * near) / (far - near);
			return projection_b / (ndc_depth - projection_a);
		}

		float32_t3 ReconstructViewPosition(uint32_t2 pixel_position, uint32_t2 resolution, float32_t depth) {

			float32_t2 ndc_position = (float32_t2(pixel_position) + 0.5) / float32_t2(resolution) * 2.0 - 1.0;
			ndc_position.y = -ndc_position.y; //!< NDCのY軸は上方向が正なので反転する

			float32_t4 clip_position = float32_t4(ndc_position, depth, 1.0);
			float32_t4 unprojected = mul(clip_position, projection_inverse);

			return unprojected.xyz / unprojected.w;
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


