#include "TaskHandle.h"
SXAVENGER_ENGINE_USING_(Scheduler)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TaskHandle structure methods
////////////////////////////////////////////////////////////////////////////////////////////

TaskState::State TaskHandle::GetState() const {
	StreamLogger::Assert(pointer_ != nullptr, "task handle has no state.");
	return pointer_->Get();
}

void TaskHandle::Wait() const {
	if (pointer_ == nullptr) {
		StreamLogger::Warning("TaskHandle | task handle has no state. wait is ignored.");
		return; //!< stateがnullptrの場合は何もしない.
	}

	while (pointer_->Get() != TaskState::State::Completed) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); //!< 1ms待機.
	}
}
