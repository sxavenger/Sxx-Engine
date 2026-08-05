#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <type_traits>
#include <concepts>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Concept namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Concept {

	////////////////////////////////////////////////////////////////////////////////////////////
	// concepts
	////////////////////////////////////////////////////////////////////////////////////////////

	template <typename T>
	concept Integral = std::is_integral_v<T> && !std::is_same_v<T, bool>;

	template <typename T>
	concept FloatingPoint = std::is_floating_point_v<T>;

	template <typename T>
	concept Arithmetic = Integral<T> || FloatingPoint<T>;

	template <typename T>
	concept Boolean = std::is_same_v<T, bool>;

	template <typename T>
	concept Enum = std::is_enum_v<T>;

	template <typename T>
	concept StringA = std::same_as<std::remove_cvref_t<T>, std::string> || std::same_as<std::remove_cvref_t<T>, std::string_view>;

	template <typename T>
	concept StringW = std::same_as<std::remove_cvref_t<T>, std::wstring> || std::same_as<std::remove_cvref_t<T>, std::wstring_view>;

	template <typename T>
	concept FunctionPointer = std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

}
