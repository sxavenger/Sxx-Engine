#include "GpuTaskScheduler.h"
SXAVENGER_ENGINE_USING_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// [GpuTaskScheduler] Condition structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void GpuTaskScheduler::Condition::Notify(Graphics::GraphicsCommandType type) {
	uint8_t index = GpuTaskScheduler::ConvertQueueIndex(type);
	conditions[index].notify_one(); //!< typeに対応する条件変数に通知する.
}

void GpuTaskScheduler::Condition::NotifyAll() {
	for (auto& condition : conditions) { //!< 全ての条件変数に通知する.
		condition.notify_all();
	}
}

void GpuTaskScheduler::Condition::Wait(Graphics::GraphicsCommandType type, const Predicate& predicate) {
	uint8_t index = GpuTaskScheduler::ConvertQueueIndex(type);
	std::unique_lock<std::mutex> lock(mutex);
	conditions[index].wait(lock, predicate); //!< typeに対応する条件変数で待機する.
}

////////////////////////////////////////////////////////////////////////////////////////////
// GpuTaskScheduler class methods
////////////////////////////////////////////////////////////////////////////////////////////

TaskHandle GpuTaskScheduler::Push(const std::shared_ptr<GpuTask>& task) {
	Graphics::GraphicsCommandType type = task->GetType();

	queue_[GpuTaskScheduler::ConvertQueueIndex(type)].emplace(task); //!< typeに対応するキューにtaskを追加する.
	condition_.Notify(type); //!< typeに対応するthreadに通知する.

	return task->GetTaskHandle(); //!< taskのTaskHandleを返す.
}

std::shared_ptr<GpuTask> GpuTaskScheduler::Pop(Graphics::GraphicsCommandType type) {
	uint8_t index = GpuTaskScheduler::ConvertQueueIndex(type);

	if (queue_[index].empty()) {
		return nullptr; //!< typeに対応するキューが空の場合はnullptrを返す.
	}

	//!< typeに対応するキューの先頭のtaskを取得する.
	std::shared_ptr<GpuTask> task = queue_[index].top();
	queue_[index].pop();
	return task;
}

bool GpuTaskScheduler::HasTask(Graphics::GraphicsCommandType type) const {
	uint8_t index = GpuTaskScheduler::ConvertQueueIndex(type);
	return !queue_[index].empty();
}

uint8_t GpuTaskScheduler::ConvertQueueIndex(Graphics::GraphicsCommandType type) {
	switch (type) {
		case Graphics::GraphicsCommandType::Compute:
		case Graphics::GraphicsCommandType::Copy:
			return EnumUtil<Graphics::GraphicsCommandType>::Cast(type) - 1; //!< Compute, Copyの2種類を管理するため, Directは除外する.

		default:
			STREAM_EXCEPTION("invalid Async GraphicsCommandType");
	}
}
