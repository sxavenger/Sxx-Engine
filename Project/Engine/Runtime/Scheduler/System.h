#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scheduler
#include "Common/TaskHandle.h"
#include "Cpu/CpuTask.h"
#include "Cpu/CpuScheduler.h"
#include "Gpu/GpuTask.h"
#include "Gpu/GpuScheduler.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// System class
////////////////////////////////////////////////////////////////////////////////////////////
class System final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Shutdown();

	//* cpu scheduler option *//

	static TaskHandle PushTask(const std::shared_ptr<CpuTask>& task);

	static TaskHandle PushTask(const CpuTask& task);

	static TaskHandle PushCpuTask(const std::string_view& name, const CpuTask::Function& function, uint8_t priority = static_cast<uint8_t>(TaskPriority::Default), TaskState::Pointer state = nullptr);

	//* gpu scheduler option *//

	static TaskHandle PushTask(const std::shared_ptr<GpuTask>& task);

	static TaskHandle PushTask(const GpuTask& task);

	static TaskHandle PushCopyQueueTask(const std::string_view& name, const GpuTask::Function& function, uint8_t priority = static_cast<uint8_t>(TaskPriority::Default));

	static TaskHandle PushComputeQueueTask(const std::string_view& name, const GpuTask::Function& function, uint8_t priority = static_cast<uint8_t>(TaskPriority::Default));

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline CpuScheduler cpu_;
	static inline GpuScheduler gpu_;

};

SXAVENGER_ENGINE_NAMESPACE_END
