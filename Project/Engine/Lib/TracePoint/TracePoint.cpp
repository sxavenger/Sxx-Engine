#include "TracePoint.h"

////////////////////////////////////////////////////////////////////////////////////////////
// TracePoint structure methods
////////////////////////////////////////////////////////////////////////////////////////////

TracePoint TracePoint::Current(std::source_location location) {
	TracePoint point = {};
	point.location  = location;
	point.id        = std::this_thread::get_id();
	point.timestamp = LocalTimePoint::Now();

	return point;
}
