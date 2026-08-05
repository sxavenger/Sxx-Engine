#include "ComponentHandle.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentHandle class methods
////////////////////////////////////////////////////////////////////////////////////////////

ComponentHandle::Type ComponentHandle::GetHandle() const {
	StreamLogger::Assert(HasHandle(), "component handle has no handle.");
	return handle_.value();
}
