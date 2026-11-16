#include "SkeletalAnimation.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletalAnimation class methods
////////////////////////////////////////////////////////////////////////////////////////////

const SkeletalAnimation::Description& SkeletalAnimation::GetDescription() const {
	Scheduler::TaskHandle handle = BaseAsset::GetTaskHandle();

	if (handle != Scheduler::TaskState::State::Completed) {
		STREAM_LOG_WARNING("Assets::SkeletalAnimation | SkeletalAnimation is not ready. name: {}, state: {}", GetName(), handle.GetState());
		handle.Wait(); //!< SkeletalAnimationが準備できるまで待機
	}

	return description_;
}
