#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <random>
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Random class
////////////////////////////////////////////////////////////////////////////////////////////
class Random {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* uniform distribution *//

	template <std::floating_point T>
	static T UniformDistribution(T min, T max) {
		//! https://cpprefjp.github.io/reference/random/uniform_real_distribution.html
		std::uniform_real_distribution<T> dist(min, max);
		return dist(seed_);
	}

	template <std::integral T>
	static T UniformDistribution(T min, T max) {
		//! https://cpprefjp.github.io/reference/random/uniform_int_distribution.html
		std::uniform_int_distribution<T> dist(min, max);
		return dist(seed_);
	}

	//* normal distribution *//

	template <std::floating_point T>
	static T NormalDistribution(T mean = 0.0, T stddev = 0.5) {
		//! https://cpprefjp.github.io/reference/random/normal_distribution.html
		std::normal_distribution<T> dist(mean, stddev);
		return dist(seed_);
	}

	template <std::floating_point T>
	static T NormalDistributionRange(T min, T max) {
		T value = Random::NormalDistribution<T>(static_cast<T>(0.0), static_cast<T>(0.3));

		T distribution
			= std::min<T>(std::abs(value), static_cast<T>(1.0));

		return min + (max - min) * distribution;
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline std::mt19937 seed_ = std::mt19937(std::random_device{}());

};
