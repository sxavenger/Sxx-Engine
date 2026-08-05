#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scheduler
#include "../Common/WorkerThread.h"
#include "GpuTask.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/GraphicsUtil.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// GpuWorkerThread class
////////////////////////////////////////////////////////////////////////////////////////////
class GpuWorkerThread final {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Function = std::function<std::shared_ptr<GpuTask>(const GpuWorkerThread&)>; //!< taskを取得する関数.

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(Graphics::GraphicsCommandType type, const Function& function);

	void Notify(bool running);

	void Shutdown();

	Graphics::GraphicsCommandType GetType() const noexcept { return type_; }

	bool IsShutdown() const noexcept { return !thread_.IsRunning(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* graphics command context *//

	Graphics::GraphicsCommandType type_;

	WorkerThread thread_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void ThreadProcMain(const Function& function) const;

};

SXAVENGER_ENGINE_NAMESPACE_END
