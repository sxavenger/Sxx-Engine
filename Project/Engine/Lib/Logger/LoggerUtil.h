#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Time/LocalTimePoint.h>
#include <Lib/TracePoint/TracePoint.h>

//* c++
#include <cstdint>
#include <thread>
#include <concepts>
#include <format>
#include <string>
#include <source_location>

////////////////////////////////////////////////////////////////////////////////////////////
// LoggerUtil namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace LoggerUtil {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Level enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Level : uint8_t {
		Debug,
		Info,
		Warning,
		Error,
		Critical
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// constants
	////////////////////////////////////////////////////////////////////////////////////////////

	static inline const std::thread::id kMainThreadId = std::this_thread::get_id(); //!< メインスレッドのidを保持する定数.

	//-----------------------------------------------------------------------------------------
	// concepts
	//-----------------------------------------------------------------------------------------

	//! @brief std::formatでフォーマット可能な型を判定するconcept.
	template <typename T>
	concept FormatA = requires(T value, std::format_context & ctx) {
		typename std::formatter<std::remove_cvref_t<T>, char>;
		std::formatter<std::remove_cvref_t<T>, char>{}.format(value, ctx);
	};

	//! @brief std::formatでフォーマット可能な型を判定するconcept.
	template <typename T>
	concept FormatW = requires(T value, std::wformat_context & ctx) {
		typename std::formatter<std::remove_cvref_t<T>, wchar_t>;
		std::formatter<std::remove_cvref_t<T>, wchar_t>{}.format(value, ctx);
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// serialize methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief Levelを文字列に変換する. (ex. Level::Debug -> "Debug")
	std::string_view SerializeLevelA(Level level) noexcept;

	//! @brief Levelを文字列に変換する. (ex. Level::Debug -> L"Debug")
	std::wstring_view SerializeLevelW(Level level) noexcept;

	//! @brief LocalTimePointを文字列に変換する. (ex. "0000-11-22 33:44:55")
	std::string SerializeTimestampA(const LocalTimePoint& time);

	//! @brief LocalTimePointを文字列に変換する. (ex. L"0000-11-22 33:44:55")
	std::wstring SerializeTimestampW(const LocalTimePoint& time);

	//! @brief std::thread::idを文字列に変換する. (ex. "main thread" or "thread id: 00000")
	std::string SerializeThreadstampA(std::thread::id id);

	//! @brief std::thread::idを文字列に変換する. (ex. L"main thread" or L"thread id: 00000")
	std::wstring SerializeThreadstampW(std::thread::id id);

	//! @brief std::source_locationを1行の文字列に変換する. (ex. "filename.cpp:123")
	std::string SerializeLocationOneLineA(const std::source_location& location);

	//! @brief std::source_locationを1行の文字列に変換する. (ex. L"filename.cpp:123")
	std::wstring SerializeLocationOneLineW(const std::source_location& location);

}
