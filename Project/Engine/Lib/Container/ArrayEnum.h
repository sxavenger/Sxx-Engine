#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Traits/Concept.h>

//* c++
#include <array>
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// ArrayEnum class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t S, Concept::Enum Enum>
class ArrayEnum {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Iterator = typename std::array<T, S>::iterator;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* access methods *//

	constexpr T& At(Enum index) { return data_.at(std::to_underlying(index)); }
	constexpr const T& At(Enum index) const { return data_.at(std::to_underlying(index)); }

	constexpr T& operator[](Enum index) { return data_[std::to_underlying(index)]; }
	constexpr const T& operator[](Enum index) const { return data_[std::to_underlying(index)]; }

	constexpr T& Front() noexcept { return data_.front(); }
	constexpr const T& Front() const noexcept { return data_.front(); }

	constexpr T& Back() noexcept { return data_.back(); }
	constexpr const T& Back() const noexcept { return data_.back(); }

	constexpr T* Data() noexcept { return data_.data(); }
	constexpr const T* Data() const noexcept { return data_.data(); }

	//* iterator methods *//

	constexpr Iterator begin() noexcept { return data_.begin(); }
	constexpr const Iterator begin() const noexcept { return data_.begin(); }

	constexpr Iterator end() noexcept { return data_.end(); }
	constexpr const Iterator end() const noexcept { return data_.end(); }

	constexpr Iterator rbegin() noexcept { return data_.rbegin(); }
	constexpr const Iterator rbegin() const noexcept { return data_.rbegin(); }

	constexpr Iterator rend() noexcept { return data_.rend(); }
	constexpr const Iterator rend() const noexcept { return data_.rend(); }

	//* capacity methods *//

	constexpr std::size_t Empty() const noexcept { return data_.empty(); }

	constexpr std::size_t Size() const noexcept { return data_.size(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<T, S> data_;

};
