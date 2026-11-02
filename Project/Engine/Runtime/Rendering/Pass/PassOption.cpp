#include "PassOption.h"
SXAVENGER_ENGINE_USING_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// PassOption structure methods
////////////////////////////////////////////////////////////////////////////////////////////

float PassOption::GetUpscaleScaleFactor(UpscalerMode mode) {
	switch (mode) {
		case UpscalerMode::Native:           return 1.0f;
		case UpscalerMode::UltraQuality:     return 1.3f;
		case UpscalerMode::Quality:          return 1.5f;
		case UpscalerMode::Balanced:         return 1.7f;
		case UpscalerMode::Performance:      return 2.0f;
		case UpscalerMode::UltraPerformance: return 3.0f;
	}

	return 1.0f; //!< デフォルトはNativeとする.
}

float PassOption::GetLimitUpscaleScaleFactor(Upscaler upscaler) {
	switch (upscaler) {
		case Upscaler::NIS:  return 2.0f; //!< NVIDIA Image Scaling (NIS) のupscale可能な最大倍率制約
		case Upscaler::FSR1: return 2.0f; //!< AMD FidelityFX Super Resolution 1.0 (FSR1) のupscale可能な最大倍率制約
		default:             return std::numeric_limits<float>::max();
	}
}

float PassOption::GetUpscaleRaito() const {
	return std::min(PassOption::GetUpscaleScaleFactor(upscalerMode), PassOption::GetLimitUpscaleScaleFactor(upscaler));
	//!< アップスケーリング倍率は、UpscalerModeの倍率とUpscalerの制約の小さい方を採用する.
}
