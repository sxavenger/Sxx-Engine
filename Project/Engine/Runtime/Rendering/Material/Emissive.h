#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Descriptor/Descriptor.h>

//* lib
#include <Lib/Math/Color3.h>

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
	// Emissive structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Emissive final {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Operation
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Operation : uint32_t {
			Color,
			Texture,
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		Emissive() noexcept = default;

		//* static methods *//

		static Emissive FromColor(const Color3f& _color, float _intensity = 0.0f) noexcept;

		static Emissive FromTexture(const Graphics::Descriptor::Handle& _handle, float _intensity = 0.0f) noexcept;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Operation operation   = Operation::Color;
		Color3f color         = Color3f{};
		uint32_t textureIndex = NULL;
		float intensity       = 0.0f;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
