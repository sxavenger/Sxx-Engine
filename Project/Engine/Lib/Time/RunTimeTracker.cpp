#include "RunTimeTracker.h"

////////////////////////////////////////////////////////////////////////////////////////////
// RunTimeTracker class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RunTimeTracker::Start() noexcept {
	reference_ = std::chrono::steady_clock::now();
	measured_.Reset();
}

void RunTimeTracker::Stop() noexcept {
	const auto now     = std::chrono::steady_clock::now();
	const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_).count();
	measured_ = TimePointd<TimeUnit::Microsecond>(static_cast<double>(elapsed));
}

TimePointd<TimeUnit::Microsecond> RunTimeTracker::GetElapsedTime() const noexcept {
	const auto now     = std::chrono::steady_clock::now();
	const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_).count();
	return TimePointd<TimeUnit::Microsecond>(static_cast<double>(elapsed));
}

TimePointd<TimeUnit::Microsecond> RunTimeTracker::GetMeasuredTime() const noexcept {
	return measured_;
}
