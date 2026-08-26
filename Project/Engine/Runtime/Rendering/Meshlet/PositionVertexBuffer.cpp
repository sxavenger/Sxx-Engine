#include "PositionVertexBuffer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PositionVertexBuffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

PositionVertexBuffer PositionVertexBuffer::Create(uint32_t capacity, uint8_t frameCount) {
	return PositionVertexBuffer{ Graphics::Core::CreateDimensionBuffer<Vector3f>(capacity, frameCount) };
}
