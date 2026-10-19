#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* pass
#include "PassContext.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Command/GraphicsCommandContext.h>

//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// BaseRenderPass class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	BaseRenderPass()          = default;
	virtual ~BaseRenderPass() = default;

	virtual void Setup(const PassContext& context) = 0;

	virtual void Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) = 0;

protected:

	//=========================================================================================
	// protected methods
	//=========================================================================================

	static void BeginEvent(Graphics::GraphicsCommandContext& command, const std::string_view& event);

	static void EndEvent(Graphics::GraphicsCommandContext& command);

};

//-----------------------------------------------------------------------------------------
// concepts
//-----------------------------------------------------------------------------------------
template <typename T>
concept RenderPass = std::is_base_of_v<BaseRenderPass, T>;

SXAVENGER_ENGINE_NAMESPACE_END
