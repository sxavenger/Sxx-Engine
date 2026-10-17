#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <cstdint>
#include <limits>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Scheduler)

////////////////////////////////////////////////////////////////////////////////////////////
// TaskPriority structure
////////////////////////////////////////////////////////////////////////////////////////////
struct TaskPriority final {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Type = uint8_t;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Priority enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Priority : Type {
		Default = 0,
		High    = std::numeric_limits<Type>::max()
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	TaskPriority() noexcept : value(static_cast<Type>(Priority::Default)) {}

	//* operator [assignment] <Priority> *//

	TaskPriority(const Priority& priority) noexcept : value(static_cast<Type>(priority)) {}
	TaskPriority& operator=(const Priority& priority) noexcept { value = static_cast<Type>(priority); return *this; }

	//* operator [assignment] <Type> *//
	
	TaskPriority(const Type& type) noexcept : value(type) {}
	TaskPriority& operator=(const Type& type) noexcept { value = type; return *this; }

	//* operator [cast] <Type> *//

	operator Type() const noexcept { return value; }
	
	//=========================================================================================
	// public variables
	//=========================================================================================

	Type value = static_cast<Type>(Priority::Default);

};

SXAVENGER_ENGINE_NAMESPACE_END
