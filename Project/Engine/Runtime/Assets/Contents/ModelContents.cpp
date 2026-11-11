#include "ModelContents.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelContents structure methods
////////////////////////////////////////////////////////////////////////////////////////////

const Uuid& ModelContents::GetMeshMaterialId(const Uuid& meshId) const {
	STREAM_ASSERT(meshMaterialMap.contains(meshId), "mesh id is not found in material.");
	return meshMaterialMap.at(meshId);
}
