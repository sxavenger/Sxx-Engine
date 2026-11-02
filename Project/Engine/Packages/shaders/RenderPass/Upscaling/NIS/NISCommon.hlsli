#pragma once

// # reference
// [NVIDIA Image Scaling (NIS)](https://github.com/NVIDIAGameWorks/NVIDIAImageScaling.git)

////////////////////////////////////////////////////////////////////////////////////////////
// NIS namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace NIS {

	////////////////////////////////////////////////////////////////////////////////////////////
	// HDRMode enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Config.h#L84-L89
	enum class HDRMode : uint32_t {
		None   = 0,
		Linear = 1,
		PQ     = 2,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Config structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief NISコンフィグ構造体
	//! @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Config.h#L45-L82
	struct Config {

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t2 input_viewport_origin;
		uint32_t2 input_viewport_size;

		uint32_t2 output_viewport_origin;
		uint32_t2 output_viewport_size;

		float32_t2 scale;
		float32_t2 dst_norm;
		float32_t2 src_norm;

		float32_t detect_ratio;
		float32_t detect_thres;
		float32_t min_contrast_ratio;
		float32_t ratio_norm;

		float32_t contrast_boost;
		float32_t eps;
		float32_t sharp_start_y;
		float32_t sharp_scale_y;

		float32_t sharp_strength_min;
		float32_t sharp_strength_scale;
		float32_t sharp_limit_min;
		float32_t sharp_limit_scale;

		//=========================================================================================
		// public methods
		//=========================================================================================

	//! @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Config.h#L156-L254
		static Config NVScalerUpdateConfig(
			float32_t sharpness,
			uint32_t2 input_viewport_origin, uint32_t2 input_viewport_size, uint32_t2 input_texture_size,
			uint32_t2 output_viewport_origin, uint32_t2 output_viewport_size, uint32_t2 output_texture_size,
			HDRMode hdr_mode) {

				Config config = (Config)0;

				// adjust params based on value from sharpness slider
				sharpness = max(min(1.0, sharpness), 0.0);
				float32_t sharpen_slider = sharpness - 0.5;   // Map 0 to 1 to -0.5 to +0.5

				// Different range for 0 to 50% vs 50% to 100%
				// The idea is to make sure sharpness of 0% map to no-sharpening,
				// while also ensuring that sharpness of 100% doesn't cause too much over-sharpening.
				const float32_t MaxScale = (sharpen_slider >= 0.0) ? 1.25 : 1.75;
				const float32_t MinScale = (sharpen_slider >= 0.0) ? 1.25 : 1.0;
				const float32_t LimitScale = (sharpen_slider >= 0.0) ? 1.25 : 1.0;

				float32_t kDetectRatio = 2 * 1127.0 / 1024.0;

				// Params for SDR
				float32_t kDetectThres      = 64.0 / 1024.0;
				float32_t kMinContrastRatio = 2.0;
				float32_t kMaxContrastRatio = 10.0;

				float32_t kSharpStartY       = 0.45;
				float32_t kSharpEndY        = 0.9;
				float32_t kSharpStrengthMin = max(0.0, 0.4 + sharpen_slider * MinScale * 1.2);
				float32_t kSharpStrengthMax = 1.6 + sharpen_slider * MaxScale * 1.8;
				float32_t kSharpLimitMin    = max(0.1, 0.14 + sharpen_slider * LimitScale * 0.32);
				float32_t kSharpLimitMax    = 0.5 + sharpen_slider * LimitScale * 0.6;

				if (hdr_mode == HDRMode::Linear || hdr_mode == HDRMode::PQ) {
					kDetectThres = 32.0 / 1024.0;

					kMinContrastRatio = 1.5;
					kMaxContrastRatio = 5.0;

					kSharpStrengthMin = max(0.0, 0.4 + sharpen_slider * MinScale * 1.1);
					kSharpStrengthMax = 2.2 + sharpen_slider * MaxScale * 1.8;
					kSharpLimitMin = max(0.06, 0.10 + sharpen_slider * LimitScale * 0.28);
					kSharpLimitMax = 0.6 + sharpen_slider * LimitScale * 0.6;

					if (hdr_mode == HDRMode::PQ) {
						kSharpStartY = 0.35;
						kSharpEndY   = 0.55;

					} else {
						kSharpStartY = 0.3;
						kSharpEndY   = 0.5;
					}
				}

				float32_t kRatioNorm          = 1.0 / (kMaxContrastRatio - kMinContrastRatio);
				float32_t kSharpScaleY        = 1.0 / (kSharpEndY - kSharpStartY);
				float32_t kSharpStrengthScale = kSharpStrengthMax - kSharpStrengthMin;
				float32_t kSharpLimitScale    = kSharpLimitMax - kSharpLimitMin;

				config.input_viewport_size  = any(input_viewport_size == 0)  ? input_texture_size  : input_viewport_size;
				config.output_viewport_size = any(output_viewport_size == 0) ? output_texture_size : output_viewport_size;

				config.input_viewport_origin  = input_viewport_origin;
				config.output_viewport_origin = output_viewport_origin;

				config.src_norm             = 1.0 / float32_t2(input_texture_size);
				config.dst_norm             = 1.0 / float32_t2(output_texture_size);
				config.scale                = float32_t2(config.input_viewport_size) / float32_t2(config.output_viewport_size);
				config.detect_ratio         = kDetectRatio;
				config.detect_thres         = kDetectThres;
				config.min_contrast_ratio   = kMinContrastRatio;
				config.ratio_norm           = kRatioNorm;
				config.contrast_boost       = 1.0;
				config.eps                  = 1.0 / 255.0;
				config.sharp_start_y        = kSharpStartY;
				config.sharp_scale_y        = kSharpScaleY;
				config.sharp_strength_min   = kSharpStrengthMin;
				config.sharp_strength_scale = kSharpStrengthScale;
				config.sharp_limit_min      = kSharpLimitMin;
				config.sharp_limit_scale    = kSharpLimitScale;

				return config;
			}
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Bindings structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Main.hlsl#L86-L97
	struct Bindings {

		//=========================================================================================
		// public variables
		//=========================================================================================

#if NIS_NV12_SUPPORT //!< NV12サポート有効時
		Texture2D<float32_t> in_texture_y;
		Texture2D<float32_t2> in_texture_uv;
#else
		Texture2D<float32_t4> in_texture;
#endif

		RWTexture2D<float32_t4> out_texture;

#if NIS_SCALER //!< スケーラー有効時
		Texture2D<float32_t4> coef_scaler;
		Texture2D<float32_t4> coef_usm;
#endif

		SamplerState sampler_linear_clamp;

	};

}
