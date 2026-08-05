#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <concepts>
#include <numbers>
#include <limits>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// Mathematical namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Math {

	////////////////////////////////////////////////////////////////////////////////////////////
	// constant variables
	////////////////////////////////////////////////////////////////////////////////////////////

	 // π
	template <std::floating_point T>
	inline constexpr T kPi = std::numbers::pi_v<T>;

	inline constexpr float  kPif = kPi<float>;
	inline constexpr double kPid = kPi<double>;

	// τ = 2π
	template <std::floating_point T>
	inline constexpr T kTau = T{2} * kPi<T>;

	inline constexpr float  kTauf = kTau<float>;
	inline constexpr double kTaud = kTau<double>;

	// 角度変換係数
	template <std::floating_point T>
	inline constexpr T kRadToDeg = T{180} / kPi<T>;

	template <std::floating_point T>
	inline constexpr T kDegToRad = kPi<T> / T{180};

	inline constexpr float kRadToDegf = kRadToDeg<float>;
	inline constexpr float kDegToRadf = kDegToRad<float>;

	// 許容誤差
	template <std::floating_point T>
	inline constexpr T kEpsilon = std::numeric_limits<T>::epsilon();

	inline constexpr float  kEpsilonf = kEpsilon<float>;
	inline constexpr double kEpsilond = kEpsilon<double>;
} 

