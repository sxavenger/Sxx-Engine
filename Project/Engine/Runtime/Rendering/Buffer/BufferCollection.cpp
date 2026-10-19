#include "BufferCollection.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// BufferCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BufferCollection::Resize(const Vector2u& resolution) {
	for (const auto& instance : instances_.GetContainer() | std::views::values) {
		instance->Resize(resolution);
	}
}

void BufferCollection::ClearTarget(Graphics::GraphicsCommandContext& command) {
	for (const auto& instance : instances_.GetContainer() | std::views::values) {
		instance->ClearTarget(command);
	}
}
