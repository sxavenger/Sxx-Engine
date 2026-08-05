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
#include <Runtime/Graphics/GraphicsUtil.h>
#include <Runtime/Graphics/Core/GraphicsCommandContext.h>

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
// GpuTask structure
////////////////////////////////////////////////////////////////////////////////////////////
struct GpuTask {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Function = std::function<void(Graphics::GraphicsCommandContext&)>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* task execution *//

	void Execute(Graphics::GraphicsCommandContext& context);

	//* task option *//

	Graphics::GraphicsCommandType GetType() const noexcept { return type; }

	uint8_t GetPriority() const noexcept { return priority; }

	//* staate option *//

	TaskHandle GetTaskHandle() const noexcept { return state; }

	//* create task methods *//

	static GpuTask CreateTask(Graphics::GraphicsCommandType type, const std::string_view& name, const Function& function, uint8_t priority = static_cast<uint8_t>(TaskPriority::Default));

	static GpuTask CreateComputeTask(const std::string_view& name, const Function& function, uint8_t priority = static_cast<uint8_t>(TaskPriority::Default));

	static GpuTask CreateCopyTask(const std::string_view& name, const Function& function, uint8_t priority = static_cast<uint8_t>(TaskPriority::Default));

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::string name = "unnamed task"; //!< タスク名

	Graphics::GraphicsCommandType type = Graphics::GraphicsCommandType::Compute; //!< コマンドタイプ
	uint8_t priority                   = static_cast<uint8_t>(TaskPriority::Default); //!< 優先度

	Function function; //!< 実行関数

	TaskState state;

	//* constants *//

	static const uint8_t kQueueCount = EnumUtil<Graphics::GraphicsCommandType>::Cast(Graphics::GraphicsCommandType::Copy); //!< AsyncQueueはCompute, Copyの2種類を管理.

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
