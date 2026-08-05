#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scheduler
#include "GpuTask.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/GraphicsUtil.h>

//* c++
#include <functional>
#include <mutex>
#include <condition_variable>
#include <array>
#include <queue>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// GpuTaskScheduler class
////////////////////////////////////////////////////////////////////////////////////////////
class GpuTaskScheduler final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Priority structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Priority {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* operator [comparison] <std::shared_ptr<GpuTask>> *//

		bool operator()(const std::shared_ptr<GpuTask>& lhs, const std::shared_ptr<GpuTask>& rhs) const {
			return lhs->GetPriority() < rhs->GetPriority(); //!< 優先度が高い方を優先する.
		}

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Condition structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Condition {
	public:

		//-----------------------------------------------------------------------------------------
		// using
		//-----------------------------------------------------------------------------------------
		using Predicate = std::function<bool()>; //!< 条件変数で待機する際の条件判定関数.

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Notify(Graphics::GraphicsCommandType type);

		void NotifyAll();

		void Wait(Graphics::GraphicsCommandType type, const Predicate& predicate);

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::mutex mutex;
		std::array<std::condition_variable, GpuTask::kQueueCount> conditions;

	};

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------
	using Queue = std::priority_queue<std::shared_ptr<GpuTask>, std::vector<std::shared_ptr<GpuTask>>, Priority>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* queue option *//

	TaskHandle Push(const std::shared_ptr<GpuTask>& task);

	std::shared_ptr<GpuTask> Pop(Graphics::GraphicsCommandType type);

	bool HasTask(Graphics::GraphicsCommandType type) const;

	//* condition option *//

	void Notify(Graphics::GraphicsCommandType type) { condition_.Notify(type); }

	void NotifyAll() { condition_.NotifyAll(); }

	void Wait(Graphics::GraphicsCommandType type, const Condition::Predicate& predicate) { condition_.Wait(type, predicate); }

	static uint8_t ConvertQueueIndex(Graphics::GraphicsCommandType type);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<Queue, GpuTask::kQueueCount> queue_; //!< typeに対応するキューを管理する配列.
	Condition condition_;

};

SXAVENGER_ENGINE_NAMESPACE_END
