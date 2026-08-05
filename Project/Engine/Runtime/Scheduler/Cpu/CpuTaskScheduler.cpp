#include "CpuTaskScheduler.h"
SXAVENGER_ENGINE_USING_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// [CpuTaskScheduler] Condition structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void CpuTaskScheduler::Condition::Notify() {
	condition.notify_one();
}

void CpuTaskScheduler::Condition::NotifyAll() {
	condition.notify_all();
}

void CpuTaskScheduler::Condition::Wait(const Predicate& predicate) {
	std::unique_lock<std::mutex> lock(mutex);
	condition.wait(lock, predicate);
}

////////////////////////////////////////////////////////////////////////////////////////////
// CpuTaskScheduler class methods
////////////////////////////////////////////////////////////////////////////////////////////

TaskHandle CpuTaskScheduler::Push(const std::shared_ptr<CpuTask>& task) {
	queue_.emplace(task); //!< taskをqueueに追加する.
	condition_.Notify(); //!< threadに通知する.

	return task->GetTaskHandle(); //!< taskのTaskHandleを返す.
}

std::shared_ptr<CpuTask> CpuTaskScheduler::Pop() {
	if (queue_.empty()) {
		return nullptr; //!< queueが空の場合はnullptrを返す.
	}

	std::shared_ptr<CpuTask> task = queue_.top(); //!< queueの先頭のtaskを取得する.
	queue_.pop();

	return task;
}

bool CpuTaskScheduler::HasTask() const {
	return !queue_.empty();
}
