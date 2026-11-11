#include "Bone.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Bone class methods
////////////////////////////////////////////////////////////////////////////////////////////

const Bone::Description& Bone::GetDescription() const {
	Scheduler::TaskHandle handle = BaseAsset::GetTaskHandle();

	if (handle != Scheduler::TaskState::State::Completed) {
		STREAM_LOG_WARNING("Assets::Bone | bone is not ready. name: {}, state: {}", GetName(), handle.GetState());
		handle.Wait(); //!< boneが準備できるまで待機
	}

	return description_;
}
