#include "GpuWorkerThread.h"
SXAVENGER_ENGINE_USING_(Scheduler)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>

//* lib
#include <Lib/Reflection/EnumUtil.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GpuWorkerThread class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GpuWorkerThread::Create(Graphics::GraphicsCommandType type, const Function& function) {

	//!< contextの取得
	type_ = type;

	//!< threadの作成
	thread_.Start(
		std::format("GpuWorkerThread <{}>", type),
		std::bind(&GpuWorkerThread::ThreadProcMain, this, function)
	);
}

void GpuWorkerThread::Notify(bool running) {
	thread_.Notify(running);
}

void GpuWorkerThread::Shutdown() {
	thread_.Shutdown();
}

void GpuWorkerThread::ThreadProcMain(const Function& function) const {
	//!< taskの取得
	std::shared_ptr<GpuTask> task = function(*this);

	if (task == nullptr) {
		//!< taskがnullptrの場合はスリープして再度ループする
		std::this_thread::yield();
		return;
	}

	task->Execute(Graphics::Core::GetCommandContext(type_)); //!< taskの実行
	Graphics::Core::SubmitQueueWait(type_); //!< command queueのsubmit
}
