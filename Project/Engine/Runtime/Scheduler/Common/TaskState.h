#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <atomic>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// TaskState structure
////////////////////////////////////////////////////////////////////////////////////////////
struct TaskState {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// State enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class State : uint8_t {
		Idle,
		Executing,
		Completed,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Data structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Data {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Set(State _state) { state.store(_state, std::memory_order::release); }

		State Get() const { return state.load(std::memory_order::acquire); }

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::atomic<State> state; //!< taskの状態

	};

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Pointer = std::shared_ptr<Data>; //!< handle側と共有するためのポインタ型


public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	TaskState() noexcept : pointer(std::make_shared<Data>()) {}
	~TaskState() noexcept = default;

	//* state option *//

	void Set(State _state) const { pointer->Set(_state); }

	State Get() const { return pointer->Get(); }

	//* operator [copy / move] <TaskState> *//

	TaskState(const TaskState& other) noexcept : pointer(other.pointer) {}
	TaskState& operator=(const TaskState& other) noexcept { pointer = other.pointer; return *this; }

	TaskState(TaskState&& other) noexcept : pointer(std::move(other.pointer)) {}
	TaskState& operator=(TaskState&& other) noexcept { pointer = std::move(other.pointer); return *this; }

	//* operator [copy] <Pointer> *//

	TaskState(const Pointer& other) noexcept : pointer(other) {}
	TaskState& operator=(const Pointer& other) noexcept { pointer = other; return *this; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	Pointer pointer;

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
