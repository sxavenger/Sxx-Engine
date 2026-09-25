#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// Util::Hash namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Util::Hash {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief 16bit版hash結合
	inline std::uint16_t Combine(std::uint16_t seed, std::uint16_t hash) noexcept {
		return seed ^ (hash + 0x9e37 + (seed << 5) + (seed >> 3));
	}

	//! @brief 32bit版hash結合
	inline std::uint32_t Combine(std::uint32_t seed, std::uint32_t hash) noexcept {
		return seed ^ (hash + 0x9e3779b9 + (seed << 6) + (seed >> 2));
	}

	//! @brief 64bit版hash結合
	inline std::uint64_t Combine(std::uint64_t seed, std::uint64_t hash) noexcept {
		return seed ^ (hash + 0x9e3779b97f4a7c15 + (seed << 12) + (seed >> 4));
	}

}
