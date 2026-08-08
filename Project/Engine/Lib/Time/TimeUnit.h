#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// TimeUnit enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class TimeUnit : uint8_t {
	Microsecond,
	Millisecond,
	Second
};

////////////////////////////////////////////////////////////////////////////////////////////
// TimeUtil namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace TimeUtil {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Factor <floating_point> structure
	////////////////////////////////////////////////////////////////////////////////////////////

	template <TimeUnit, std::floating_point>
	struct FactorFloatingPoint;

	template <std::floating_point T>
	struct FactorFloatingPoint<TimeUnit::Microsecond, T> {
		static constexpr T factor = T(1.0); //!< microsecond factor (基準単位として扱う.)
	};

	template <std::floating_point T>
	struct FactorFloatingPoint<TimeUnit::Millisecond, T> {
		static constexpr T factor = T(1e3);
	};

	template <std::floating_point T>
	struct FactorFloatingPoint<TimeUnit::Second, T> {
		static constexpr T factor = T(1e6);
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Factor <integral> structure
	////////////////////////////////////////////////////////////////////////////////////////////

	template <TimeUnit, std::integral>
	struct FactorIntegral;

	template <std::integral T>
	struct FactorIntegral<TimeUnit::Microsecond, T> {
		static constexpr T factor = T(1); //!< microsecond factor (基準単位として扱う.)
	};

	template <std::integral T>
	struct FactorIntegral<TimeUnit::Millisecond, T> {
		static constexpr T factor = T(1e3);
	};

	template <std::integral T>
	struct FactorIntegral<TimeUnit::Second, T> {
		static constexpr T factor = T(1e6);
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Conversion functions
	////////////////////////////////////////////////////////////////////////////////////////////

	template <TimeUnit From, TimeUnit To, std::floating_point T>
	constexpr T ConvertFactorFloatingPoint() {
		constexpr T from = FactorFloatingPoint<From, T>::factor;
		constexpr T to   = FactorFloatingPoint<To, T>::factor;

		return from / to;
	}

	template <TimeUnit From, TimeUnit To, std::integral T>
	constexpr T ConvertFactorIntegral() {
		constexpr T from = FactorIntegral<From, T>::factor;
		constexpr T to   = FactorIntegral<To, T>::factor;

		return from / to;
	}

}




