#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scheduler
#include "../Common/TaskState.h"
#include "../Common/TaskHandle.h"
#include "../Common/TaskPriority.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Reflection/EnumUtil.h>

//* c++
#include <functional>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// CpuTask structure
////////////////////////////////////////////////////////////////////////////////////////////
struct CpuTask {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Function = std::function<void()>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* task execution *//

	void Execute();

	//* task option *//

	uint8_t GetPriority() const noexcept { return priority; }

	//* staate option *//

	TaskHandle GetTaskHandle() const noexcept { return state; }

	//* create task methods *//

	static CpuTask CreateTask(const std::string_view& name, const Function& function, uint8_t priority = static_cast<uint8_t>(TaskPriority::Default), TaskState::Pointer state = nullptr);

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::string name = "unnamed task"; //!< タスク名

	uint8_t priority = static_cast<uint8_t>(TaskPriority::Default); //!< 優先度

	Function function; //!< 実行関数

	TaskState state;

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
