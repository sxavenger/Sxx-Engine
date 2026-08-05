#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <thread>
#include <atomic>
#include <functional>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// WorkerThread class
////////////////////////////////////////////////////////////////////////////////////////////
class WorkerThread final {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------
	using Function = std::function<void()>; //!< スレッドで実行する関数.

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	WorkerThread() noexcept = default;
	~WorkerThread();

	//* thread option *//

	void Start(const std::string_view& name, const Function& function);

	void Notify(bool running) { running_.store(running, std::memory_order::release); }

	void Shutdown();

	bool IsRunning() const noexcept { return running_.load(std::memory_order::acquire); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::thread thread_;
	std::atomic<bool> running_ = false;

	std::string name_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void ThreadProcLoop(const Function& function);

};

SXAVENGER_ENGINE_NAMESPACE_END
