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

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	BaseBuffer() noexcept          = default;
	virtual ~BaseBuffer() noexcept = default;

	//* buffer option *//

	virtual void Create(const Vector2u& resolution) = 0;

	virtual void Resize(const Vector2u& resolution) = 0;

	virtual void ClearTarget(Graphics::GraphicsCommandContext& command) = 0;

protected:
};

//-----------------------------------------------------------------------------------------
// concepts
//-----------------------------------------------------------------------------------------
template <typename T>
concept Buffer = std::derived_from<T, BaseBuffer>;

SXAVENGER_ENGINE_NAMESPACE_END
