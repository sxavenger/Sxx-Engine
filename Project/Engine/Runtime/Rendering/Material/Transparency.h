#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
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
	// Transparency structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Transparency final {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Operation
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Operation : uint32_t {
			Transparency,
			Texture,
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		Transparency() noexcept = default;

		//* static methods *//

		static Transparency FromValue(float _transparency) noexcept;

		static Transparency FromTexture(const Graphics::Descriptor::Handle& _handle) noexcept;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Operation operation   = Operation::Transparency;
		float transparency    = 1.0f;
		uint32_t textureIndex = NULL;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
