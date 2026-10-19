#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/GraphicsUtil.h>
#include <Runtime/Graphics/Descriptor/Descriptor.h>

//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// Material namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Material {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Roughness structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Roughness final {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Operation
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Operation : uint32_t {
			Value,
			Texture,
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		Roughness() noexcept = default;

		//* static methods *//

		static Roughness FromValue(float _value) noexcept;

		static Roughness FromTexture(const Graphics::Descriptor::Handle& _handle, Graphics::TextureChannel _channel = Graphics::TextureChannel::G) noexcept;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Operation operation                     = Operation::Value;
		float value                             = 0.5f;
		uint32_t textureIndex                   = NULL;
		Graphics::TextureChannel textureChannel = Graphics::TextureChannel::G;
		//!< ARMTexture想定でRoughnessはGチャンネルに格納されることを想定しているため設定している

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
