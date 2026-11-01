#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/SwapChain.h>

//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// PassOption structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PassOption {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// WorkingColorSpace enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class WorkingColorSpace : uint32_t {
		Linear = 0,
		ACEScg = 1,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// DisplayEncoder enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class DisplayEncoder : uint32_t {
		Software = 0,
		Hardware = 1,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Upscaler enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Upscaler : uint8_t {
		Bilinear, //!< Bilinear
		NIS,      //!< NVIDIA Image Scaling (NIS)
		FSR1,     //!< AMD FidelityFX Super Resolution 1.0 (FSR1)
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// UpscalerMode enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class UpscalerMode : uint8_t {
		Native,
		UltraQuality,
		Quality,
		Balanced,
		Performance,
		UltraPerformance,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// AmbientOcclusion enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class AmbientOcclusion : uint8_t {
		None,
		XeGTAO,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// AntiAliasing enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class AntiAliasing : uint8_t {
		None,
		FXAA,
		SMAA_1x
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* upscaler option *//

	static float GetUpscaleScaleFactor(UpscalerMode mode);

	static float GetLimitUpscaleScaleFactor(Upscaler upscaler);

	float GetUpscaleRaito() const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	WorkingColorSpace workingColorSpace           = WorkingColorSpace::ACEScg;
	Graphics::DisplayColorSpace displayColorSpace = Graphics::DisplayColorSpace::Rec709;
	DisplayEncoder displayEncoder                 = DisplayEncoder::Hardware;

	Upscaler upscaler         = Upscaler::Bilinear;
	UpscalerMode upscalerMode = UpscalerMode::Native;

	AmbientOcclusion ambientOcclusion = AmbientOcclusion::None;
	AntiAliasing antiAliasing = AntiAliasing::None; // TODO: FXAAに変更する.

};

SXAVENGER_ENGINE_NAMESPACE_END
