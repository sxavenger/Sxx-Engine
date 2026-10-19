#include "SurfaceMaterialBuffer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceMaterialBuffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

SurfaceMaterialBuffer SurfaceMaterialBuffer::Create(uint8_t frameCount) {
	return SurfaceMaterialBuffer{ Graphics::Core::CreateConstantBuffer<SurfaceMaterialBuffer::Description>(frameCount) };
}
