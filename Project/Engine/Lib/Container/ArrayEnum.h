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
// ArrayEnum structure
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t S, Concept::Enum Enum>
struct ArrayEnum final {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Container = std::array<T, S>;

	using iterator               = Container::iterator;
	using const_iterator         = Container::const_iterator;
	using reverse_iterator       = Container::reverse_iterator;
	using const_reverse_iterator = Container::const_reverse_iterator;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* access methods *//

	constexpr T& At(Enum index) { return data.at(std::to_underlying(index)); }
	constexpr const T& At(Enum index) const { return data.at(std::to_underlying(index)); }

	constexpr T& At(size_t index) { return data.at(index); }
	constexpr const T& At(size_t index) const { return data.at(index); }

	constexpr T& operator[](Enum index) { return data[std::to_underlying(index)]; }
	constexpr const T& operator[](Enum index) const { return data[std::to_underlying(index)]; }

	constexpr T& operator[](size_t index) { return data[index]; }
	constexpr const T& operator[](size_t index) const { return data[index]; }

	constexpr T& Front() noexcept { return data.front(); }
	constexpr const T& Front() const noexcept { return data.front(); }

	constexpr T& Back() noexcept { return data.back(); }
	constexpr const T& Back() const noexcept { return data.back(); }

	constexpr T* Data() noexcept { return data.data(); }
	constexpr const T* Data() const noexcept { return data.data(); }

	//* iterator methods *//

	constexpr iterator begin() noexcept { return data.begin(); }
	constexpr const_iterator begin() const noexcept { return data.begin(); }

	constexpr iterator end() noexcept { return data.end(); }
	constexpr const_iterator end() const noexcept { return data.end(); }

	constexpr reverse_iterator rbegin() noexcept { return data.rbegin(); }
	constexpr const_reverse_iterator rbegin() const noexcept { return data.rbegin(); }

	constexpr reverse_iterator rend() noexcept { return data.rend(); }
	constexpr const_reverse_iterator rend() const noexcept { return data.rend(); }

	//* capacity methods *//

	constexpr std::size_t Empty() const noexcept { return data.empty(); }

	constexpr std::size_t Size() const noexcept { return data.size(); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	Container data;

};
