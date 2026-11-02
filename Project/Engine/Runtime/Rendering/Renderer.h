#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "Pass/PassOption.h"
#include "Pass/RenderPassPipeline.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Command/GraphicsCommandContext.h>
#include <Runtime/Scheduler/Common/TaskHandle.h>

//* c++
#include <queue>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// Renderer class
////////////////////////////////////////////////////////////////////////////////////////////
class Renderer final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Cache();

	static void Render(Graphics::GraphicsCommandContext& command, const PassContext& context);

	//* render pipeline option *//

	template <RenderPass T>
	static void InsertRenderPass();

	//* render option *//

	static void SetOption(const PassOption& option) { option_ = option; }

	static PassOption& GetOption() { return option_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* cache parameter *//

	//static inline std::queue<Scheduler::TaskHandle> cacheHandleQueue_;

	//* render pipeline *//

	static inline PassOption option_;
	static inline RenderPassPipeline pipeline_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Renderer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <RenderPass T>
void Renderer::InsertRenderPass() {
	pipeline_.Insert<T>();
}

SXAVENGER_ENGINE_NAMESPACE_END
