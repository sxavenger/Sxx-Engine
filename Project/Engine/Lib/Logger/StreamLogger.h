#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "LoggerUtil.h"

//* lib
#include <Lib/CXXAttribute.hpp>

//* c++
#include <mutex>

////////////////////////////////////////////////////////////////////////////////////////////
// StreamLogger class
////////////////////////////////////////////////////////////////////////////////////////////
class StreamLogger final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* output stamp methods *//

	//! @brief 各loggerにstamp付きの文字列を出力する.
	static void OutputStampA(LoggerUtil::Level level, const TracePoint& point, const std::string_view& message);

	//! @brief 各loggerにstamp付きの文字列を出力する.
	static void OutputStampW(LoggerUtil::Level level, const TracePoint& point, const std::wstring_view& message);

	//* log override methods *//

	static void Log(LoggerUtil::Level level, const TracePoint& point, const std::string_view& message);

	static void Log(LoggerUtil::Level level, const TracePoint& point, const std::wstring_view& message);

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	static void Log(LoggerUtil::Level level, const TracePoint& point, std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	static void Log(LoggerUtil::Level level, const TracePoint& point, std::wformat_string<Args...> format, Args&&... args);

	//* exception methods *//

	NORETURN static void ExceptionA(const TracePoint& point, const std::string_view& message);

	NORETURN static void ExceptionW(const TracePoint& point, const std::wstring_view& message);

	NORETURN static void ExceptionSummaryA(const TracePoint& point, const std::string_view& summary, const std::string_view& message);

	NORETURN static void ExceptionSummaryW(const TracePoint& point, const std::wstring_view& summary, const std::wstring_view& message);

	//* exception override methods *//

	NORETURN static void Exception(const TracePoint& point, const std::string_view& message);

	NORETURN static void Exception(const TracePoint& point, const std::wstring_view& message);

	template  <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	NORETURN static void Exception(const TracePoint& point, std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	NORETURN static void Exception(const TracePoint& point, std::wformat_string<Args...> format, Args&&... args);

	NORETURN static void ExceptionSummary(const TracePoint& point, const std::string_view& summary, const std::string_view& message);

	NORETURN static void ExceptionSummary(const TracePoint& point, const std::wstring_view& summary, const std::wstring_view& message);

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	NORETURN static void ExceptionSummary(const TracePoint& point, const std::string_view& summary, std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	NORETURN static void ExceptionSummary(const TracePoint& point, const std::wstring_view& summary, std::wformat_string<Args...> format, Args&&... args);

	//* assert methods *//

	static void AssertA(bool expression, const TracePoint& point, const std::string_view& message);

	static void AssertW(bool expression, const TracePoint& point, const std::wstring_view& message);

	static void AssertSummaryA(bool expression, const TracePoint& point, const std::string_view& summary, const std::string_view& message);

	static void AssertSummaryW(bool expression, const TracePoint& point, const std::wstring_view& summary, const std::wstring_view& message);

	//* assert override methods *//

	static void Assert(bool expression, const TracePoint& point, const std::string_view& message);

	static void Assert(bool expression, const TracePoint& point, const std::wstring_view& message);

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	static void Assert(bool expression, const TracePoint& point, std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	static void Assert(bool expression, const TracePoint& point, std::wformat_string<Args...> format, Args&&... args);

	static void AssertSummary(bool expression, const TracePoint& point, const std::string_view& summary, const std::string_view& message);

	static void AssertSummary(bool expression, const TracePoint& point, const std::wstring_view& summary, const std::wstring_view& message);

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	static void AssertSummary(bool expression, const TracePoint& point, const std::string_view& summary, std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	static void AssertSummary(bool expression, const TracePoint& point, const std::wstring_view& summary, std::wformat_string<Args...> format, Args&&... args);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline std::mutex mutex_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* output methods *//

	static void OutputA(const std::string_view& message);

	static void OutputW(const std::wstring_view& message);

	//* separator methods *//

	static void SeparatorA(const char separator = '#', size_t len = 128);

	static void SeparatorW(const wchar_t separator = L'#', size_t len = 128);

	//* endline methods *//

	static void Endline();

	//* serialize option *//

	static std::string GetLocationMessageA(const std::source_location& location);

	static std::wstring GetLocationMessageW(const std::source_location& location);

	static std::string GetThreadMessageA(const std::thread::id id);

	static std::wstring GetThreadMessageW(const std::thread::id id);

	static std::string GetTimestampMessageA(const LocalTimePoint& time);

	static std::wstring GetTimestampMessageW(const LocalTimePoint& time);

};

////////////////////////////////////////////////////////////////////////////////////////////
// StreamLogger class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
inline void StreamLogger::Log(LoggerUtil::Level level, const TracePoint& point, std::format_string<Args...> format, Args&&... args) {
	StreamLogger::OutputStampA(level, point, std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
inline void StreamLogger::Log(LoggerUtil::Level level, const TracePoint& point, std::wformat_string<Args...> format, Args&&... args) {
	StreamLogger::OutputStampW(level, point, std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
NORETURN inline void StreamLogger::Exception(const TracePoint& point, std::format_string<Args...> format, Args&&... args) {
	StreamLogger::ExceptionA(point, std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
NORETURN inline void StreamLogger::Exception(const TracePoint& point, std::wformat_string<Args...> format, Args&&... args) {
	StreamLogger::ExceptionW(point, std::format(format, std::forward<Args>(args)...));
}

inline void StreamLogger::ExceptionSummary(const TracePoint& point, const std::string_view& summary, const std::string_view& message) {
	StreamLogger::ExceptionSummaryA(point, summary, message);
}

inline void StreamLogger::ExceptionSummary(const TracePoint& point, const std::wstring_view& summary, const std::wstring_view& message) {
	StreamLogger::ExceptionSummaryW(point, summary, message);
}

template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
NORETURN inline void StreamLogger::ExceptionSummary(const TracePoint& point, const std::string_view& summary, std::format_string<Args...> format, Args&&... args) {
	StreamLogger::ExceptionSummaryA(point, summary, std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
NORETURN inline void StreamLogger::ExceptionSummary(const TracePoint& point, const std::wstring_view& summary, std::wformat_string<Args...> format, Args&&... args) {
	StreamLogger::ExceptionSummaryW(point, summary, std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
inline void StreamLogger::Assert(bool expression, const TracePoint& point, std::format_string<Args...> format, Args&&... args) {
	StreamLogger::AssertA(expression, point, std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
inline void StreamLogger::Assert(bool expression, const TracePoint& point, std::wformat_string<Args...> format, Args&&... args) {
	StreamLogger::AssertW(expression, point, std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
inline void StreamLogger::AssertSummary(bool expression, const TracePoint& point, const std::string_view& summary, std::format_string<Args...> format, Args&&... args) {
	StreamLogger::AssertSummaryA(expression, point, summary, std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
inline void StreamLogger::AssertSummary(bool expression, const TracePoint& point, const std::wstring_view& summary, std::wformat_string<Args...> format, Args&&... args) {
	StreamLogger::AssertSummaryW(expression, point, summary, std::format(format, std::forward<Args>(args)...));
}

//-----------------------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------------------

// Logging macros

#define STREAM_LOG_DEBUG(format, ...)    StreamLogger::Log(LoggerUtil::Level::Debug, TracePoint(), format, __VA_ARGS__)
#define STREAM_LOG_INFO(format, ...)     StreamLogger::Log(LoggerUtil::Level::Info, TracePoint(), format, __VA_ARGS__)
#define STREAM_LOG_WARNING(format, ...)  StreamLogger::Log(LoggerUtil::Level::Warning, TracePoint(), format, __VA_ARGS__)
#define STREAM_LOG_ERROR(format, ...)    StreamLogger::Log(LoggerUtil::Level::Error, TracePoint(), format, __VA_ARGS__)
#define STREAM_LOG_CRITICAL(format, ...) StreamLogger::Log(LoggerUtil::Level::Critical, TracePoint(), format, __VA_ARGS__)

// Exception macros

#define STREAM_EXCEPTION(format, ...)                  StreamLogger::Exception(TracePoint(), format, __VA_ARGS__)
#define STREAM_EXCEPTION_SUMMARY(summary, format, ...) StreamLogger::ExceptionSummary(TracePoint(), summary, format, __VA_ARGS__)

// Assert macros

#define STREAM_ASSERT(expression, format, ...)                  StreamLogger::Assert(expression, TracePoint(), format, __VA_ARGS__)
#define STREAM_ASSERT_SUMMARY(expression, summary, format, ...) StreamLogger::AssertSummary(expression, TracePoint(), summary, format, __VA_ARGS__)
