#include "GpuTask.h"
SXAVENGER_ENGINE_USING_(Scheduler)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/EnumUtil.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GpuTask structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void GpuTask::Execute(Graphics::GraphicsCommandContext& context) {
	if (function == nullptr) {
		STREAM_LOG_ERROR("Scheduler::GpuTask | execute function is null. name: {}", name);
		return; //!< functionがnullptrの場合は実行しない.
	}

	if (state.Get() != TaskState::State::Idle) {
		STREAM_LOG_WARNING("Scheduler::GpuTask | execute function is not idle. name: {}, state: {}", name, state.Get());
		return; //!< stateがIdleでない場合は実行しない.
	}

	state.Set(TaskState::State::Executing);

	function(context);

	state.Set(TaskState::State::Completed);
}

GpuTask GpuTask::CreateTask(Graphics::GraphicsCommandType type, const std::string_view& name, const Function& function, uint8_t priority) {
	GpuTask task = {};
	task.type     = type;
	task.name     = name;
	task.priority = priority;
	task.function = function;

	return task;
}

GpuTask GpuTask::CreateComputeTask(const std::string_view& name, const Function& function, uint8_t priority) {
	return GpuTask::CreateTask(Graphics::GraphicsCommandType::Compute, name, function, priority);
}

GpuTask GpuTask::CreateCopyTask(const std::string_view& name, const Function& function, uint8_t priority) {
	return GpuTask::CreateTask(Graphics::GraphicsCommandType::Copy, name, function, priority);
}
