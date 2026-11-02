#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/ResourceHandle.h>
#include <Runtime/Graphics/Descriptor/Descriptor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// UpscalerNIS class
////////////////////////////////////////////////////////////////////////////////////////////
class UpscalerNIS final {
	//! @reference [NVIDIA Image Scaling (NIS)](https://github.com/NVIDIAGameWorks/NVIDIAImageScaling.git)
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct TextureData final {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* descriptor option *//

		const Graphics::Descriptor& GetDescriptor() const { return descriptor; }

		//* static method *//

		static TextureData Create(const std::string_view& name, const void* data);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Graphics::ResourceHandle handle;
		Graphics::Descriptor descriptor;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Setup();

	//* coefficients options *//

	const TextureData& GetCoefficientsScale() const { return coefficientsScale_; }

	const TextureData& GetCoefficientsUsm() const { return coefficientsUsm_; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	constexpr static inline uint32_t kBlockWidth  = 32; //!< NIS_BLOCK_WIDTH と一致させる
	constexpr static inline uint32_t kBlockHeight = 24; //!< NIS_BLOCK_HEIGHT と一致させる

private:

	//=========================================================================================
	// public variables
	//=========================================================================================

	TextureData coefficientsScale_;
	TextureData coefficientsUsm_;

};

SXAVENGER_ENGINE_NAMESPACE_END
