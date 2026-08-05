#include "CpuScheduler.h"
SXAVENGER_ENGINE_USING_(Scheduler)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CpuScheduler class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CpuScheduler::Init() {

	//!< threadの作成
	for (auto& thread : threads_) {
		thread.Create(std::bind(&CpuScheduler::GetTaskProc, this, std::placeholders::_1)); //!< threadの作成
	}

	StreamLogger::Info("Scheduler::CpuScheduler | initialized.");
}

void CpuScheduler::Shutdown() {
	for (auto& thread : threads_) {
		thread.Notify(false); //!< threadの待機状態を解除する.
	}

	scheduler_.NotifyAll(); //!< 全ての条件変数に通知する.

	for (auto& thread : threads_) {
		thread.Shutdown(); //!< threadのシャットダウン
	}

	StreamLogger::Info("Scheduler::CpuScheduler | shutdown.");
}

TaskHandle CpuScheduler::PushTask(const std::shared_ptr<CpuTask>& task) {
	return scheduler_.Push(task); //!< taskをqueueに追加する.
}

std::shared_ptr<CpuTask> CpuScheduler::GetTaskProc(const CpuWorkerThread& thread) {

	scheduler_.Wait([&]() { return thread.IsShutdown() || scheduler_.HasTask(); }); //!< typeに対応する条件変数で待機する.

	if (thread.IsShutdown()) {
		return nullptr; //!< threadがシャットダウンされている場合はnullptrを返す.
	}

	return scheduler_.Pop(); //!< typeに対応するキューからtaskを取得する.
}
