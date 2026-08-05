#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scheduler
#include "TaskState.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// TaskHandle structure
////////////////////////////////////////////////////////////////////////////////////////////
class TaskHandle {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	TaskHandle() noexcept = default;

	//* handle option *//

	void Reset() noexcept { pointer_.reset(); }

	bool HasState() const noexcept { return pointer_ != nullptr; }

	TaskState::State GetState() const;

	void Wait() const;

	//* operator [copy / move] <TaskHandle> *//

	TaskHandle(const TaskHandle&) noexcept            = default;
	TaskHandle& operator=(const TaskHandle&) noexcept = default;

	TaskHandle(TaskHandle&&) noexcept            = default;
	TaskHandle& operator=(TaskHandle&&) noexcept = default;

	//* operator [copy] <TaskState> *//

	TaskHandle(const TaskState& _state) noexcept : pointer_(_state.pointer) {}
	TaskHandle& operator=(const TaskState& _state) noexcept { pointer_ = _state.pointer; return *this; }

	//* operator [comparison] <std::nullptr_t> *//

	bool operator==(std::nullptr_t) const noexcept { return pointer_ == nullptr; }
	bool operator!=(std::nullptr_t) const noexcept { return pointer_ != nullptr; }

	//* operator [comparison] <TaskState::State> *//

	bool operator==(TaskState::State _state) const noexcept { return pointer_ != nullptr && pointer_->Get() == _state; }
	bool operator!=(TaskState::State _state) const noexcept { return pointer_ == nullptr || pointer_->Get() != _state; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	TaskState::Pointer pointer_;

};

SXAVENGER_ENGINE_NAMESPACE_END
