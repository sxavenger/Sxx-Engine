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
	// Metallic structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Metallic final {
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

		Metallic() noexcept = default;

		//* static methods *//

		static Metallic FromValue(float _value) noexcept;

		static Metallic FromTexture(const Graphics::Descriptor::Handle& _handle, Graphics::TextureChannel _channel = Graphics::TextureChannel::B) noexcept;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Operation operation                     = Operation::Value;
		float value                             = 0.0f;
		uint32_t textureIndex                   = NULL;
		Graphics::TextureChannel textureChannel = Graphics::TextureChannel::B;
		//!< ARMTexture想定でMetallicはBチャンネルに格納されることを想定しているため設定している

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
