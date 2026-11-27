#include "MeshVertexInfluenceBuffer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshVertexInfluenceBuffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

MeshVertexInfluenceBuffer MeshVertexInfluenceBuffer::Create(uint32_t capacity, uint8_t frameCount) {
	return MeshVertexInfluenceBuffer{ Graphics::Core::CreateDimensionBuffer<MeshVertexInfluenceBuffer::Influence>(capacity, frameCount) };
}
