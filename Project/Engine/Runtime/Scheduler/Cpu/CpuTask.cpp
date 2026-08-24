#include "CpuTask.h"
SXAVENGER_ENGINE_USING_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// CpuTask structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void CpuTask::Execute() {
	if (function == nullptr) {
		STREAM_LOG_ERROR("Scheduler::CpuTask | execute function is null. name: {}", name);
		return; //!< functionがnullptrの場合は実行しない.
	}

	if (state.Get() != TaskState::State::Idle) {
		STREAM_LOG_WARNING("Scheduler::CpuTask | execute function is not idle. name: {}, state: {}", name, state.Get());
		return; //!< stateがIdleでない場合は実行しない.
	}

	state.Set(TaskState::State::Executing);

	function(); //!< function(task)の実行
	
	state.Set(TaskState::State::Completed);
}

CpuTask CpuTask::CreateTask(const std::string_view& name, const Function& function, uint8_t priority, TaskState::Pointer state) {
	CpuTask task = {};
	task.name     = name;
	task.priority = priority;
	task.function = function;

	if (state != nullptr) {
		task.state = state; //!< stateを上書きする.
	}
	

	return task;
}
