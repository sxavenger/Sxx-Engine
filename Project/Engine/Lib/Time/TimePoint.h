#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* time
#include "TimeUnit.h"

//* c++
#include <concepts>
#include <algorithm>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// TimePoint structure
////////////////////////////////////////////////////////////////////////////////////////////
template <TimeUnit Unit, std::floating_point T>
struct TimePoint {
	// TODO: Integer型を扱うTimePointIntegral型を作成する.
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr TimePoint() noexcept = default;

	//* time point methods *//

	constexpr void Reset() noexcept { time = {}; }

	//* operator [assignment] <T> *//

	constexpr TimePoint(T time) noexcept : time(time) {}
	constexpr TimePoint& operator=(T time) noexcept { this->time = time; return *this; }

	//* operator [assignment / move] <TimePoint> *//

	constexpr TimePoint(const TimePoint&) noexcept            = default;
	constexpr TimePoint& operator=(const TimePoint&) noexcept = default;

	constexpr TimePoint(TimePoint&&) noexcept            = default;
	constexpr TimePoint& operator=(TimePoint&&) noexcept = default;

	//* operator [conversion] <T> *//

	constexpr operator T() const noexcept { return time; }

	//* operator [conversion] <TimePoint<U, U>> *//

	template <TimeUnit To>
	constexpr operator TimePoint<To, T>() const noexcept {
		return TimePoint<To, T>(time * TimeUtil::ConvertFactor<Unit, To, T>());
	}

	template <std::floating_point U>
	constexpr operator TimePoint<Unit, U>() const noexcept {
		return TimePoint<Unit, U>(static_cast<U>(time));
	}

	template <TimeUnit To, std::floating_point U>
	constexpr operator TimePoint<To, U>() const noexcept {
		return TimePoint<To, U>(static_cast<U>(time) * TimeUtil::ConvertFactor<Unit, To, U>());
	}

	//* operator [arithmetic] <TimePoint> *//

	constexpr TimePoint operator+(const TimePoint& rhs) const noexcept { return TimePoint(time + rhs.time); }
	constexpr TimePoint& operator+=(const TimePoint& rhs) noexcept { time += rhs.time; return *this; }

	constexpr TimePoint operator-(const TimePoint& rhs) const noexcept { return TimePoint(time - rhs.time); }
	constexpr TimePoint& operator-=(const TimePoint& rhs) noexcept { time -= rhs.time; return *this; }

	//* operator [arithmetic] <T> *//

	constexpr TimePoint operator*(T v) const noexcept { return TimePoint(time * v); }
	constexpr TimePoint& operator*=(T v) noexcept { time *= v; return *this; }

	constexpr TimePoint operator/(T v) const noexcept { return TimePoint(time / v); }
	constexpr TimePoint& operator/=(T v) noexcept { time /= v; return *this; }

	//* operator [comparison] <TimePoint> *//

	constexpr bool operator==(const TimePoint& rhs) const noexcept { return time == rhs.time; }
	constexpr bool operator!=(const TimePoint& rhs) const noexcept { return time != rhs.time; }

	constexpr bool operator<(const TimePoint& rhs) const noexcept { return time < rhs.time; }
	constexpr bool operator<=(const TimePoint& rhs) const noexcept { return time <= rhs.time; }

	constexpr bool operator>(const TimePoint& rhs) const noexcept { return time > rhs.time; }
	constexpr bool operator>=(const TimePoint& rhs) const noexcept { return time >= rhs.time; }

	//* mathmatical methods *//

	static constexpr TimePoint Min(const TimePoint& x, const TimePoint& y) noexcept { return std::min(x, y); }
	static constexpr TimePoint Max(const TimePoint& x, const TimePoint& y) noexcept { return std::max(x, y); }
	static constexpr TimePoint Clamp(const TimePoint& x, const TimePoint& min, const TimePoint& max) noexcept { return std::clamp(x, min, max); }

	static constexpr TimePoint Mod(const TimePoint& x, const TimePoint& y) noexcept { return TimePoint(std::fmod(x.time, y.time)); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	T time = {};

};

////////////////////////////////////////////////////////////////////////////////////////////
// TimePoint structure utility aliases
////////////////////////////////////////////////////////////////////////////////////////////

template <TimeUnit Unit>
using TimePointf = TimePoint<Unit, float>;

template <TimeUnit Unit>
using TimePointd = TimePoint<Unit, double>;
