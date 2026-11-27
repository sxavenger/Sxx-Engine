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
	static void OutputStampA(LoggerUtil::Level level, const Snapshot& snapshot, const std::string_view& message);

	//! @brief 各loggerにstamp付きの文字列を出力する.
	static void OutputStampW(LoggerUtil::Level level, const Snapshot& snapshot, const std::wstring_view& message);

	//* log override methods *//

	static void Log(LoggerUtil::Level level, const Snapshot& snapshot, const std::string_view& message);

	static void Log(LoggerUtil::Level level, const Snapshot& snapshot, const std::wstring_view& message);

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	static void Log(LoggerUtil::Level level, const Snapshot& snapshot, std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	static void Log(LoggerUtil::Level level, const Snapshot& snapshot, std::wformat_string<Args...> format, Args&&... args);

	//* exception methods *//

	NORETURN static void ExceptionA(const SnapshotDetailed& snapshot, const std::string_view& message);

	NORETURN static void ExceptionW(const SnapshotDetailed& snapshot, const std::wstring_view& message);

	NORETURN static void ExceptionSummaryA(const SnapshotDetailed& snapshot, const std::string_view& summary, const std::string_view& message);

	NORETURN static void ExceptionSummaryW(const SnapshotDetailed& snapshot, const std::wstring_view& summary, const std::wstring_view& message);

	//* exception override methods *//

	NORETURN static void Exception(const SnapshotDetailed& snapshot, const std::string_view& message);

	NORETURN static void Exception(const SnapshotDetailed& snapshot, const std::wstring_view& message);

	template  <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	NORETURN static void Exception(const SnapshotDetailed& snapshot, std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	NORETURN static void Exception(const SnapshotDetailed& snapshot, std::wformat_string<Args...> format, Args&&... args);

	NORETURN static void ExceptionSummary(const SnapshotDetailed& snapshot, const std::string_view& summary, const std::string_view& message);

	NORETURN static void ExceptionSummary(const SnapshotDetailed& snapshot, const std::wstring_view& summary, const std::wstring_view& message);

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	NORETURN static void ExceptionSummary(const SnapshotDetailed& snapshot, const std::string_view& summary, std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	NORETURN static void ExceptionSummary(const SnapshotDetailed& snapshot, const std::wstring_view& summary, std::wformat_string<Args...> format, Args&&... args);

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

	static std::string GetLocationMessageA(const std::filesystem::path& file, const std::string_view& function, const uint_least32_t line);

	static std::wstring GetLocationMessageW(const std::filesystem::path& file, const std::string_view& function, const uint_least32_t line);

	static std::string GetThreadMessageA(const std::thread::id id);

	static std::wstring GetThreadMessageW(const std::thread::id id);

	static std::string GetTimestampMessageA(const LocalTimePoint& time);

	static std::wstring GetTimestampMessageW(const LocalTimePoint& time);

	static std::string GetStacktraceMessageA(const std::span<const SnapshotDetailed::StackFrame>& stacktrace);

	static std::wstring GetStacktraceMessageW(const std::span<const SnapshotDetailed::StackFrame>& stacktrace);

};

////////////////////////////////////////////////////////////////////////////////////////////
// StreamLogger class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
inline void StreamLogger::Log(LoggerUtil::Level level, const Snapshot& snapshot, std::format_string<Args...> format, Args&&... args) {
	StreamLogger::OutputStampA(level, snapshot, std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
inline void StreamLogger::Log(LoggerUtil::Level level, const Snapshot& snapshot, std::wformat_string<Args...> format, Args&&... args) {
	StreamLogger::OutputStampW(level, snapshot, std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
NORETURN inline void StreamLogger::Exception(const SnapshotDetailed& snapshot, std::format_string<Args...> format, Args&&... args) {
	StreamLogger::ExceptionA(snapshot, std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
NORETURN inline void StreamLogger::Exception(const SnapshotDetailed& snapshot, std::wformat_string<Args...> format, Args&&... args) {
	StreamLogger::ExceptionW(snapshot, std::format(format, std::forward<Args>(args)...));
}

NORETURN inline void StreamLogger::ExceptionSummary(const SnapshotDetailed& snapshot, const std::string_view& summary, const std::string_view& message) {
	StreamLogger::ExceptionSummaryA(snapshot, summary, message);
}

NORETURN inline void StreamLogger::ExceptionSummary(const SnapshotDetailed& snapshot, const std::wstring_view& summary, const std::wstring_view& message) {
	StreamLogger::ExceptionSummaryW(snapshot, summary, message);
}

template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
NORETURN inline void StreamLogger::ExceptionSummary(const SnapshotDetailed& snapshot, const std::string_view& summary, std::format_string<Args...> format, Args&&... args) {
	StreamLogger::ExceptionSummaryA(snapshot, summary, std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
NORETURN inline void StreamLogger::ExceptionSummary(const SnapshotDetailed& snapshot, const std::wstring_view& summary, std::wformat_string<Args...> format, Args&&... args) {
	StreamLogger::ExceptionSummaryW(snapshot, summary, std::format(format, std::forward<Args>(args)...));
}

//-----------------------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------------------

// Logging macros

#define STREAM_LOG_DEBUG(format, ...)    StreamLogger::Log(LoggerUtil::Level::Debug,    Snapshot::Current(), format, ##__VA_ARGS__)
#define STREAM_LOG_INFO(format, ...)     StreamLogger::Log(LoggerUtil::Level::Info,     Snapshot::Current(), format, ##__VA_ARGS__)
#define STREAM_LOG_WARNING(format, ...)  StreamLogger::Log(LoggerUtil::Level::Warning,  Snapshot::Current(), format, ##__VA_ARGS__)
#define STREAM_LOG_ERROR(format, ...)    StreamLogger::Log(LoggerUtil::Level::Error,    Snapshot::Current(), format, ##__VA_ARGS__)
#define STREAM_LOG_CRITICAL(format, ...) StreamLogger::Log(LoggerUtil::Level::Critical, Snapshot::Current(), format, ##__VA_ARGS__)

// Exception macros

#define STREAM_EXCEPTION(format, ...)                  StreamLogger::Exception(SnapshotDetailed::Current(), format, ##__VA_ARGS__)
#define STREAM_EXCEPTION_SUMMARY(summary, format, ...) StreamLogger::ExceptionSummary(SnapshotDetailed::Current(), summary, format, ##__VA_ARGS__)

// Assert macros

#define STREAM_ASSERT(expression, format, ...) \
	if (!(expression)) UNLIKELY { \
		StreamLogger::Exception(SnapshotDetailed::Current(), format, ##__VA_ARGS__); \
	}

#define STREAM_ASSERT_SUMMARY(expression, summary, format, ...) \
	if (!(expression)) UNLIKELY { \
		StreamLogger::ExceptionSummary(SnapshotDetailed::Current(), summary, format, ##__VA_ARGS__); \
	}
