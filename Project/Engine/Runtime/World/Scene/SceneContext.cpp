#include "SceneContext.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneContext::Register(const std::string& name, const std::function<std::unique_ptr<BaseScene>()>& factory) {
	if (factory_.contains(name)) {
		STREAM_LOG_WARNING("World::SceneContext | scene already registered. name: {}", name);
		return;
	}

	factory_[name] = factory;
	STREAM_LOG_DEBUG("World::SceneContext | scene registered. name: {}", name);
}
