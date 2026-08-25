#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include "../Time/LocalTimePoint.h"

//* lib
#include <Lib/CXXAttribute.hpp>

//* c++
#include <thread>
#include <source_location>

////////////////////////////////////////////////////////////////////////////////////////////
// TracePoint structure
////////////////////////////////////////////////////////////////////////////////////////////
struct TracePoint {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	TracePoint() = default;

	//* operator [copy] (default) *//

	TracePoint(const TracePoint&)            = default;
	TracePoint& operator=(const TracePoint&) = default;

	//* operator [move] (default) *//

	TracePoint(TracePoint&&)            = default;
	TracePoint& operator=(TracePoint&&) = default;

	//* static methods *//

	static TracePoint Current(std::source_location location = std::source_location::current());

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::thread::id id;
	std::source_location location;
	LocalTimePoint timestamp;

	// c++23になった場合, std::stacktraceを追加する.

};
