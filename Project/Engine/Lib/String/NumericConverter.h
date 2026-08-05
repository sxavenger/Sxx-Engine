#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Traits/Concept.h>

//* c++
#include <string_view>
#include <charconv>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// NumericConverter class
////////////////////////////////////////////////////////////////////////////////////////////
class NumericConverter {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* convert option *//

	//! @brief 文字列を整数型に変換する
	//! @param[in] str 変換する文字列
	//! @param[in] base 変換する基数 (ex. 10進数:10, 16進数:16)
	//! @retval T 変換後の整数値
	//! @retval std::nullopt 変換に失敗した場合
	template <Concept::Integral T>
	static std::optional<T> ConvertIntegral(const std::string_view& str, uint8_t base = 10);

	template <Concept::FloatingPoint T>
	static std::optional<T> ConvertFloatingPoint(const std::string_view& str);

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// NumericConverter class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Concept::Integral T>
std::optional<T> NumericConverter::ConvertIntegral(const std::string_view& str, uint8_t base) {
	if (str.empty()) {
		return std::nullopt;
	}

	T value = 0;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value, base);

	if (ec != std::errc()) {
		return std::nullopt; //!< 変換に失敗した場合はstd::nulloptを返す
	}

	return value;
}

template  <Concept::FloatingPoint T>
std::optional<T> NumericConverter::ConvertFloatingPoint(const std::string_view& str) {
	if (str.empty()) {
		return std::nullopt;
	}

	T value = 0;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);

	if (ec != std::errc()) {
		return std::nullopt; //!< 変換に失敗した場合はstd::nulloptを返す
	}

	return value;
}
