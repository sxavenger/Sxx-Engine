#include "System.h"
SXAVENGER_ENGINE_USING_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// System class methods
////////////////////////////////////////////////////////////////////////////////////////////

void System::Init() {
	cpu_.Init();
	gpu_.Init();
}

void System::Shutdown() {
	cpu_.Shutdown();
	gpu_.Shutdown();
}

TaskHandle System::PushTask(const std::shared_ptr<CpuTask>& task) {
	return cpu_.PushTask(task);
}

TaskHandle System::PushTask(const CpuTask& task) {
	return cpu_.PushTask(std::make_shared<CpuTask>(task));
}

TaskHandle System::PushCpuTask(const std::string_view& name, const CpuTask::Function& function, uint8_t priority, TaskState::Pointer state) {
	return cpu_.PushTask(std::make_shared<CpuTask>(CpuTask::CreateTask(name, function, priority, state)));
}

TaskHandle System::PushTask(const std::shared_ptr<GpuTask>& task) {
	return gpu_.PushTask(task);
}

TaskHandle System::PushTask(const GpuTask& task) {
	return gpu_.PushTask(std::make_shared<GpuTask>(task));
}

TaskHandle System::PushCopyQueueTask(const std::string_view& name, const GpuTask::Function& function, uint8_t priority) {
	return gpu_.PushTask(std::make_shared<GpuTask>(GpuTask::CreateCopyTask(name, function, priority)));
}

TaskHandle System::PushComputeQueueTask(const std::string_view& name, const GpuTask::Function& function, uint8_t priority) {
	return gpu_.PushTask(std::make_shared<GpuTask>(GpuTask::CreateComputeTask(name, function, priority)));
}
