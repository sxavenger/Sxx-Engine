#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "LoggerUtil.h"

//* c++
#include <mutex>

////////////////////////////////////////////////////////////////////////////////////////////
// ConsoleLogger class
////////////////////////////////////////////////////////////////////////////////////////////
class ConsoleLogger final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	//* output methods *//

	//! @brief コンソールに文字列を出力する.
	static void OutputA(const std::string_view& message);

	//! @brief コンソールに文字列を出力する.
	static void OutputW(const std::wstring_view& message);

	//* formatter output methods *//

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	static void OutputA(std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	static void OutputW(std::wformat_string<Args...> format, Args&&... args);

	//* separator methods *//

	//! @brief コンソールに区切り線を出力する.
	static void SeparatorA(const char separator = '#', size_t len = 128);

	//! @brief コンソールに区切り線を出力する.
	static void SeparatorW(const wchar_t separator = L'#', size_t len = 128);

	//* end line methods *//

	//! @brief コンソールに空行を出力する.
	static void Endline();

	//* stamp output methods *//

	//! @brief コンソールにスタンプ付きの文字列を出力する.
	static void OutputStampA(const std::string_view& label, LoggerUtil::Level level, const TracePoint& point, const std::string_view& message);

	//! @brief コンソールにスタンプ付きの文字列を出力する.
	static void OutputStampW(const std::wstring_view& label, LoggerUtil::Level level, const TracePoint& point, const std::wstring_view& message);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* thread *//

	static inline std::mutex mutex_ = {};

};

////////////////////////////////////////////////////////////////////////////////////////////
// ConsoleLogger class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
inline void ConsoleLogger::OutputA(std::format_string<Args...> format, Args&&... args) {
	ConsoleLogger::OutputA(std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
inline void ConsoleLogger::OutputW(std::wformat_string<Args...> format, Args&&... args) {
	ConsoleLogger::OutputW(std::format(format, std::forward<Args>(args)...));
}
