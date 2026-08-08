#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* time
#include "TimeUnit.h"

//* lib
#include <Lib/CXXAttribute.hpp>

//* c++
#include <concepts>
#include <algorithm>
#include <cmath>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------

template <TimeUnit, std::integral>
struct DEPRECATED("not defined") TimePointIntegral; //!< 必要であれば整数型の検討.

template <TimeUnit, std::floating_point>
struct TimePointFloatingPoint;

////////////////////////////////////////////////////////////////////////////////////////////
// TimePointFloatingPoint structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 時間点を表す構造体 (浮動小数点型)
template <TimeUnit Unit, std::floating_point Type>
struct TimePointFloatingPoint {

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr TimePointFloatingPoint() noexcept = default;

	//* time point methods *//

	void Reset() noexcept { *this = TimePointFloatingPoint{}; }

	//* operator [copy / move] <TimePointFloatingPoint> *//

	constexpr TimePointFloatingPoint(const TimePointFloatingPoint&) noexcept            = default;
	constexpr TimePointFloatingPoint& operator=(const TimePointFloatingPoint&) noexcept = default;

	constexpr TimePointFloatingPoint(TimePointFloatingPoint&&) noexcept            = default;
	constexpr TimePointFloatingPoint& operator=(TimePointFloatingPoint&&) noexcept = default;

	//* operator [assignment] <Type> *//

	constexpr TimePointFloatingPoint(Type _time) noexcept : time(_time) {}
	constexpr TimePointFloatingPoint& operator=(Type _time) noexcept { time = _time; return *this; }

	//* operator [conversion] <TimePointFloatingPoint> *//

	template <TimeUnit T>
	constexpr operator TimePointFloatingPoint<T, Type>() const noexcept {
		return TimePointFloatingPoint<T, Type>(time * TimeUtil::ConvertFactorFloatingPoint<Unit, T, Type>());
	}

	template <std::floating_point T>
	constexpr operator TimePointFloatingPoint<Unit, T>() const noexcept {
		return TimePointFloatingPoint<Unit, T>(static_cast<T>(time));
	}

	template <TimeUnit T, std::floating_point U>
	constexpr operator TimePointFloatingPoint<T, U>() const noexcept {
		return TimePointFloatingPoint<T, U>(static_cast<U>(time) * TimeUtil::ConvertFactorFloatingPoint<Unit, T, U>());
	}

	//* operator [unary] *//

	constexpr TimePointFloatingPoint operator+() const noexcept { return *this; }
	constexpr TimePointFloatingPoint operator-() const noexcept { return TimePointFloatingPoint(-time); }

	//* operator [arithmetic] <TimePointFloatingPoint> *//

	constexpr TimePointFloatingPoint operator+(const TimePointFloatingPoint& rhs) const noexcept { return TimePointFloatingPoint(time + rhs.time); }
	constexpr TimePointFloatingPoint& operator+=(const TimePointFloatingPoint& rhs) noexcept { time += rhs.time; return *this; }

	constexpr TimePointFloatingPoint operator-(const TimePointFloatingPoint& rhs) const noexcept { return TimePointFloatingPoint(time - rhs.time); }
	constexpr TimePointFloatingPoint& operator-=(const TimePointFloatingPoint& rhs) noexcept { time -= rhs.time; return *this; }

	//* operator [arithmetic] <Type> *//

	constexpr TimePointFloatingPoint operator*(Type rhs) const noexcept { return TimePointFloatingPoint(time * rhs); }
	constexpr TimePointFloatingPoint& operator*=(Type rhs) noexcept { time *= rhs; return *this; }

	constexpr TimePointFloatingPoint operator/(Type rhs) const noexcept { return TimePointFloatingPoint(time / rhs); }
	constexpr TimePointFloatingPoint& operator/=(Type rhs) noexcept { time /= rhs; return *this; }

	//* operator [comparison] <TimePointFloatingPoint> *//

	constexpr bool operator==(const TimePointFloatingPoint& rhs) const noexcept { return time == rhs.time; }
	constexpr bool operator!=(const TimePointFloatingPoint& rhs) const noexcept { return time != rhs.time; }

	constexpr bool operator<(const TimePointFloatingPoint& rhs) const noexcept { return time < rhs.time; }
	constexpr bool operator<=(const TimePointFloatingPoint& rhs) const noexcept { return time <= rhs.time; }

	constexpr bool operator>(const TimePointFloatingPoint& rhs) const noexcept { return time > rhs.time; }
	constexpr bool operator>=(const TimePointFloatingPoint& rhs) const noexcept { return time >= rhs.time; }

	//* algorithm methods *//

	static constexpr TimePointFloatingPoint Min(const TimePointFloatingPoint& lhs, const TimePointFloatingPoint& rhs) noexcept {
		return std::min(lhs.time, rhs.time);
	}

	static constexpr TimePointFloatingPoint Max(const TimePointFloatingPoint& lhs, const TimePointFloatingPoint& rhs) noexcept {
		return std::max(lhs.time, rhs.time);
	}

	static constexpr TimePointFloatingPoint Clamp(const TimePointFloatingPoint& v, const TimePointFloatingPoint& low, const TimePointFloatingPoint& high) noexcept {
		return TimePointFloatingPoint(std::clamp(v.time, low.time, high.time));
	}

	//* mathmatical methods *//

	static constexpr TimePointFloatingPoint Abs(const TimePointFloatingPoint& v) noexcept {
		return TimePointFloatingPoint(std::abs(v.time));
	}

	static constexpr TimePointFloatingPoint Floor(const TimePointFloatingPoint& v) noexcept {
		return TimePointFloatingPoint(std::floor(v.time));
	}

	static constexpr TimePointFloatingPoint Ceil(const TimePointFloatingPoint& v) noexcept {
		return TimePointFloatingPoint(std::ceil(v.time));
	}

	static constexpr TimePointFloatingPoint Round(const TimePointFloatingPoint& v) noexcept {
		return TimePointFloatingPoint(std::round(v.time));
	}

	static constexpr TimePointFloatingPoint Mod(const TimePointFloatingPoint& v, Type divisor) noexcept {
		return TimePointFloatingPoint(std::fmod(v.time, divisor));
	}

	//=========================================================================================
	// public variables
	//=========================================================================================

	Type time{ 0.0 }; //!< 時間点 (単位は Unit に依存する.)

};

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------

template <TimeUnit Unit>
using TimePointf = TimePointFloatingPoint<Unit, float>;

template <TimeUnit Unit>
using TimePointd = TimePointFloatingPoint<Unit, double>;
