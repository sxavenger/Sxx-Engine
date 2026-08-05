#include "CpuWorkerThread.h"
SXAVENGER_ENGINE_USING_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// CpuWorkerThread class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CpuWorkerThread::Create(const Function& function) {
	//!< threadの作成
	thread_.Start(
		"CpuWorkerThread",
		std::bind(&CpuWorkerThread::ThreadProcMain, this, function)
	);
}

void CpuWorkerThread::Notify(bool running) {
	thread_.Notify(running);
}

void CpuWorkerThread::Shutdown() {
	thread_.Shutdown();
}

void CpuWorkerThread::ThreadProcMain(const Function& function) const {
	//!< taskの取得
	std::shared_ptr<CpuTask> task = function(*this);

	if (task == nullptr) {
		//!< taskがnullptrの場合はスリープして再度ループする
		std::this_thread::yield();
		return;
	}

	task->Execute(); //!< taskの実行
}
