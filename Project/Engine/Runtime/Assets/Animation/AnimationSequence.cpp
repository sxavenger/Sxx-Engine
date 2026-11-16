#include "AnimationSequence.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationSequence class methods
////////////////////////////////////////////////////////////////////////////////////////////

const AnimationSequence::Description& AnimationSequence::GetDescription() const {
	Scheduler::TaskHandle handle = BaseAsset::GetTaskHandle();

	if (handle != Scheduler::TaskState::State::Completed) {
		STREAM_LOG_WARNING("Assets::AnimationSequence | AnimationSequence is not ready. name: {}, state: {}", GetName(), handle.GetState());
		handle.Wait(); //!< AnimationSequenceが準備できるまで待機
	}

	return description_;
}
