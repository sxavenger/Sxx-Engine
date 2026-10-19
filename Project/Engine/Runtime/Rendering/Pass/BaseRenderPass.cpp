#include "BaseRenderPass.h"
SXAVENGER_ENGINE_USING_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// BaseRenderPass class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseRenderPass::BeginEvent(Graphics::GraphicsCommandContext& command, const std::string_view& event) {
	command.BeginEvent(event);
}

void BaseRenderPass::EndEvent(Graphics::GraphicsCommandContext& command) {
	command.EndEvent();
}
