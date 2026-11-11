#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "EntityHandle.h"
#include "EntityBehaviour.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Assets/Contents/ModelContents.h>

//* lib
#include <Lib/Pointer/ReferencePointer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// EntityFactory class
////////////////////////////////////////////////////////////////////////////////////////////
class EntityFactory final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* basic entity option *//

	static EntityHandle Create();

	static void Destroy(const EntityHandle& handle);

	static RefPtr<EntityBehaviour> GetEntity(const EntityHandle& handle);

	//* entity option *//

	static EntityHandle CreateModelEntity(const Assets::ModelContents& contents);

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
