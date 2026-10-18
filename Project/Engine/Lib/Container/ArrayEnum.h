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
struct ArrayEnum {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using iterator               = typename std::array<T, S>::iterator;
	using const_iterator         = typename std::array<T, S>::const_iterator;
	using reverse_iterator       = typename std::array<T, S>::reverse_iterator;
	using const_reverse_iterator = typename std::array<T, S>::const_reverse_iterator;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* access methods *//

	constexpr T& At(Enum index) { return data_.at(std::to_underlying(index)); }
	constexpr const T& At(Enum index) const { return data_.at(std::to_underlying(index)); }

	constexpr T& At(size_t index) { return data_.at(index); }
	constexpr const T& At(size_t index) const { return data_.at(index); }

	constexpr T& operator[](Enum index) { return data_[std::to_underlying(index)]; }
	constexpr const T& operator[](Enum index) const { return data_[std::to_underlying(index)]; }

	constexpr T& operator[](size_t index) { return data_[index]; }
	constexpr const T& operator[](size_t index) const { return data_[index]; }

	constexpr T& Front() noexcept { return data_.front(); }
	constexpr const T& Front() const noexcept { return data_.front(); }

	constexpr T& Back() noexcept { return data_.back(); }
	constexpr const T& Back() const noexcept { return data_.back(); }

	constexpr T* Data() noexcept { return data_.data(); }
	constexpr const T* Data() const noexcept { return data_.data(); }

	//* iterator methods *//

	constexpr iterator begin() noexcept { return data_.begin(); }
	constexpr const_iterator begin() const noexcept { return data_.begin(); }

	constexpr iterator end() noexcept { return data_.end(); }
	constexpr const_iterator end() const noexcept { return data_.end(); }

	constexpr reverse_iterator rbegin() noexcept { return data_.rbegin(); }
	constexpr const_reverse_iterator rbegin() const noexcept { return data_.rbegin(); }

	constexpr reverse_iterator rend() noexcept { return data_.rend(); }
	constexpr const_reverse_iterator rend() const noexcept { return data_.rend(); }

	//* capacity methods *//

	constexpr std::size_t Empty() const noexcept { return data_.empty(); }

	constexpr std::size_t Size() const noexcept { return data_.size(); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::array<T, S> data_;

};
