#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scheduler
#include "GpuTask.h"
#include "GpuWorkerThread.h"
#include "GpuTaskScheduler.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// GpuScheduler class
////////////////////////////////////////////////////////////////////////////////////////////
class GpuScheduler final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Shutdown();

	TaskHandle PushTask(const std::shared_ptr<GpuTask>& task);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* queue thread *//

	std::array<GpuWorkerThread, GpuTask::kQueueCount> threads_;

	GpuTaskScheduler scheduler_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::shared_ptr<GpuTask> GetTaskProc(const GpuWorkerThread& thread);

};

SXAVENGER_ENGINE_NAMESPACE_END
