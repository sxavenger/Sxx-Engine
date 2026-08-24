#include "GpuScheduler.h"
SXAVENGER_ENGINE_USING_(Scheduler)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GpuScheduler class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GpuScheduler::Init() {

	//!< threadの作成
	for (Graphics::GraphicsCommandType type : { Graphics::GraphicsCommandType::Compute, Graphics::GraphicsCommandType::Copy }) {
		threads_[GpuTaskScheduler::ConvertQueueIndex(type)].Create(type, std::bind(&GpuScheduler::GetTaskProc, this, std::placeholders::_1)); //!< threadの作成
	}

	STREAM_LOG_INFO("Scheduler::GpuScheduler | initialized.");
}

void GpuScheduler::Shutdown() {
	for (auto& thread : threads_) {
		thread.Notify(false); //!< threadの待機状態を解除する.
	}

	scheduler_.NotifyAll(); //!< 全ての条件変数に通知する.

	for (auto& thread : threads_) {
		thread.Shutdown(); //!< threadのシャットダウン
	}
	
	STREAM_LOG_INFO("Scheduler::GpuScheduler | shutdown.");
}

TaskHandle GpuScheduler::PushTask(const std::shared_ptr<GpuTask>& task) {
	return scheduler_.Push(task); //!< taskをqueueに追加する.
}

std::shared_ptr<GpuTask> GpuScheduler::GetTaskProc(const GpuWorkerThread& thread) {

	Graphics::GraphicsCommandType type = thread.GetType();

	scheduler_.Wait(type, [&]() { return thread.IsShutdown() || scheduler_.HasTask(type); }); //!< typeに対応する条件変数で待機する.

	if (thread.IsShutdown()) {
		return nullptr; //!< threadがシャットダウンされている場合はnullptrを返す.
	}

	return scheduler_.Pop(type); //!< typeに対応するキューからtaskを取得する.
}
