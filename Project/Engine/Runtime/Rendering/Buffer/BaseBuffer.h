#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Command/GraphicsCommandContext.h>

//* lib
#include <Lib/Math/Vector2.h>

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// BaseBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Resolution structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Resolution {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2u render = {}; //!< upscale前の解像度.
		Vector2u output = {}; //!< upscale後の解像度.

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	BaseBuffer() noexcept          = default;
	virtual ~BaseBuffer() noexcept = default;

	//* buffer option *//

	virtual void Create(const Resolution& resolution) = 0;

	virtual void Resize(const Resolution& resolution) = 0;

	virtual void ClearTarget(Graphics::GraphicsCommandContext& command) = 0;

protected:
};

//-----------------------------------------------------------------------------------------
// concepts
//-----------------------------------------------------------------------------------------
template <typename T>
concept Buffer = std::derived_from<T, BaseBuffer>;

SXAVENGER_ENGINE_NAMESPACE_END
