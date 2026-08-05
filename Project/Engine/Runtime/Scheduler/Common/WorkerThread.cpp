#include "WorkerThread.h"
SXAVENGER_ENGINE_USING_(Scheduler)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/String/UnicodeConverter.h>

//* windows
#include <windows.h>

////////////////////////////////////////////////////////////////////////////////////////////
// WorkerThread class methods
////////////////////////////////////////////////////////////////////////////////////////////

WorkerThread::~WorkerThread() {
	if (running_.load(std::memory_order::acquire)) {
		StreamLogger::Warning("WorkerThread - {} | thread is still running. shutdown is called.", name_);
		Shutdown();
	}
}

void WorkerThread::Start(const std::string_view& name, const Function& function) {
	if (running_.load(std::memory_order::acquire)) {
		StreamLogger::Warning("WorkerThread - {} | thread is already running. start is ignored.", name_);
		return; //!< すでにスレッドが実行中の場合は何もしない.
	}

	name_ = name;

	running_.store(true, std::memory_order::release); //!< running_をtrueに設定してスレッドを開始する.
	thread_ = std::thread(&WorkerThread::ThreadProcLoop, this, function);
	SetThreadDescription(thread_.native_handle(), std::format(L"Scheduler::WorkerThread | {}", UnicodeConverter::ConvertW(name_)).c_str());
}

void WorkerThread::Shutdown() {
	Notify(false); //!< running_をfalseに設定してスレッドを終了する.

	if (thread_.joinable()) {
		thread_.join();
	}
}

void WorkerThread::ThreadProcLoop(const Function& function) {
	StreamLogger::Info("WorkerThread - {} | thread started.", name_);

	while (running_.load(std::memory_order::acquire)) {
		function();
	}

	StreamLogger::Info("WorkerThread - {} | thread shutdown.", name_);
}
