#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* time
#include "TimePoint.h"

//* c++
#include <chrono>

////////////////////////////////////////////////////////////////////////////////////////////
// RunTimeTracker class
////////////////////////////////////////////////////////////////////////////////////////////
class RunTimeTracker {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Start() noexcept;

	void Stop() noexcept;

	//* getter *//

	//! @brief Start()からの経過時間を取得
	TimePointd<TimeUnit::Microsecond> GetElapsedTime() const noexcept;

	//! @brief Start()からStop()までの経過時間を取得
	TimePointd<TimeUnit::Microsecond> GetMeasuredTime() const noexcept;

	//! @brief Start()の基準時間を取得
	const std::chrono::steady_clock::time_point& GetReference() const noexcept { return reference_; }

private:

	//=========================================================================================
	// private members
	//=========================================================================================

	//* chrono *//

	std::chrono::steady_clock::time_point reference_;

	//* time point *//

	TimePointd<TimeUnit::Microsecond> measured_;

};
