#include "EntityHandle.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EntityHandle class methods
////////////////////////////////////////////////////////////////////////////////////////////

EntityHandle::Type EntityHandle::GetHandle() const {
	STREAM_ASSERT(HasHandle(), "entity handle has no handle.");
	return handle_.value();
}
