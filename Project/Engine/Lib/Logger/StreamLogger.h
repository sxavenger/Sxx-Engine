#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "LoggerUtil.h"

//* lib
#include "../CXXAttribute.hpp"
#include "../TracePoint/TracePoint.h"
#include "../Time/LocalTimePoint.h"

//* c++
#include <fstream>
#include <filesystem>
#include <mutex>
#include <thread>
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// StreamLogger class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ストリームログ管理クラス.
class StreamLogger final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	//* log option *//

	static void Log(LoggerUtil::Level level, const std::string_view& message);
	static void Log(LoggerUtil::Level level, const std::wstring_view& message);

	static void Debug(const std::string_view& message) { StreamLogger::Log(LoggerUtil::Level::Debug, message); }
	static void Debug(const std::wstring_view& message) { StreamLogger::Log(LoggerUtil::Level::Debug, message); }

	static void Info(const std::string_view& message) { StreamLogger::Log(LoggerUtil::Level::Info, message); }
	static void Info(const std::wstring_view& message) { StreamLogger::Log(LoggerUtil::Level::Info, message); }

	static void Warning(const std::string_view& message) { StreamLogger::Log(LoggerUtil::Level::Warning, message); }
	static void Warning(const std::wstring_view& message) { StreamLogger::Log(LoggerUtil::Level::Warning, message); }

	static void Error(const std::string_view& message) { StreamLogger::Log(LoggerUtil::Level::Error, message); }
	static void Error(const std::wstring_view& message) { StreamLogger::Log(LoggerUtil::Level::Error, message); }

	static void Critical(const std::string_view& message) { StreamLogger::Log(LoggerUtil::Level::Critical, message); }
	static void Critical(const std::wstring_view& message) { StreamLogger::Log(LoggerUtil::Level::Critical, message); }

	//* exception option *//

	NORETURN static void Exception(const std::string_view& message, const TracePoint& point = TracePoint());
	NORETURN static void Exception(const std::wstring_view& message, const TracePoint& point = TracePoint());

	NORETURN static void Exception(const std::string_view& message, const std::string_view& detail, const TracePoint& point = TracePoint());
	NORETURN static void Exception(const std::wstring_view& message, const std::wstring_view& detail, const TracePoint& point = TracePoint());

	//* assertion option *//

	static void Assert(bool expression, const std::string_view& message, const TracePoint& point = TracePoint());
	static void Assert(bool expression, const std::wstring_view& message, const TracePoint& point = TracePoint());

	static void Assert(bool expression, const std::string_view& message, const std::string_view& detail, const TracePoint& point = TracePoint());
	static void Assert(bool expression, const std::wstring_view& message, const std::wstring_view& detail, const TracePoint& point = TracePoint());

	//* format log option *//

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	static void Log(LoggerUtil::Level level, std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	static void Log(LoggerUtil::Level level, std::wformat_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	static void Debug(std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	static void Debug(std::wformat_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	static void Info(std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	static void Info(std::wformat_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	static void Warning(std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	static void Warning(std::wformat_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	static void Error(std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	static void Error(std::wformat_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	static void Critical(std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	static void Critical(std::wformat_string<Args...> format, Args&&... args);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* file *//

	static inline const std::ofstream::openmode kModeAppend = std::ofstream::out | std::ofstream::app;

	static inline const std::filesystem::path kDirectory = "Logs";
	static inline const std::filesystem::path kFilename  = LoggerUtil::CreateFilename(".log");

	//* thread *//

	static inline std::mutex mutex_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* message conversion helper methods *//

	static std::string GetLocationMessageA(const std::source_location& location);
	static std::wstring GetLocationMessageW(const std::source_location& location);

	static std::string GetThreadMessageA(const std::thread::id id);
	static std::wstring GetThreadMessageW(const std::thread::id id);

	static std::string GetTimestampMessageA(const LocalTimePoint& time);
	static std::wstring GetTimestampMessageW(const LocalTimePoint& time);

	//* logging helper methods *//

	static void OutputStampA(LoggerUtil::Level level, const std::string_view& message);
	static void OutputStampW(LoggerUtil::Level level, const std::wstring_view& message);

	static void OutputA(const std::string_view& message);
	static void OutputW(const std::wstring_view& message);

	static void OutputConsoleA(const std::string_view& message);
	static void OutputConsoleW(const std::wstring_view& message);

	static void OutputFileA(const std::string_view& message);
	static void OutputFileW(const std::wstring_view& message);

	static void OutputSeparator(const char separator = '=');
	static void OutputEndline();

	//* debug break method *//

	NORETURN static void DebugBreak();

	//* exception window method *//

	static void OpenExceptionWindowA(
		const std::string_view& message,
		const std::string_view& location,
		const std::string_view& thread,
		const std::string_view& timestamp
	);

	static void OpenExceptionWindowW(
		const std::wstring_view& location,
		const std::wstring_view& thread,
		const std::wstring_view& timestamp,
		const std::wstring_view& message
	);

};

////////////////////////////////////////////////////////////////////////////////////////////
// StreamLogger class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename ...Args> requires (LoggerUtil::FormatA<Args>&& ...)
inline void StreamLogger::Log(LoggerUtil::Level level, std::format_string<Args...> format, Args&& ...args) {
	StreamLogger::Log(level, std::format(format, std::forward<Args>(args)...));
}

template <typename ...Args> requires (LoggerUtil::FormatW<Args>&& ...)
inline void StreamLogger::Log(LoggerUtil::Level level, std::wformat_string<Args...> format, Args&& ...args) {
	StreamLogger::Log(level, std::format(format, std::forward<Args>(args)...));
}

template <typename ...Args> requires (LoggerUtil::FormatA<Args>&& ...)
inline void StreamLogger::Debug(std::format_string<Args...> format, Args&& ...args) {
	StreamLogger::Debug(std::format(format, std::forward<Args>(args)...));
}

template <typename ...Args> requires (LoggerUtil::FormatW<Args>&& ...)
inline void StreamLogger::Debug(std::wformat_string<Args...> format, Args&& ...args) {
	StreamLogger::Debug(std::format(format, std::forward<Args>(args)...));
}

template <typename ...Args> requires (LoggerUtil::FormatA<Args>&& ...)
inline void StreamLogger::Info(std::format_string<Args...> format, Args&& ...args) {
	StreamLogger::Info(std::format(format, std::forward<Args>(args)...));
}

template <typename ...Args> requires (LoggerUtil::FormatW<Args>&& ...)
inline void StreamLogger::Info(std::wformat_string<Args...> format, Args&& ...args) {
	StreamLogger::Info(std::format(format, std::forward<Args>(args)...));
}

template <typename ...Args> requires (LoggerUtil::FormatA<Args>&& ...)
inline void StreamLogger::Warning(std::format_string<Args...> format, Args&& ...args) {
	StreamLogger::Warning(std::format(format, std::forward<Args>(args)...));
}

template <typename ...Args> requires (LoggerUtil::FormatW<Args>&& ...)
inline void StreamLogger::Warning(std::wformat_string<Args...> format, Args&& ...args) {
	StreamLogger::Warning(std::format(format, std::forward<Args>(args)...));
}

template <typename ...Args> requires (LoggerUtil::FormatA<Args>&& ...)
inline void StreamLogger::Error(std::format_string<Args...> format, Args&& ...args) {
	StreamLogger::Error(std::format(format, std::forward<Args>(args)...));
}

template <typename ...Args> requires (LoggerUtil::FormatW<Args>&& ...)
inline void StreamLogger::Error(std::wformat_string<Args...> format, Args&& ...args) {
	StreamLogger::Error(std::format(format, std::forward<Args>(args)...));
}

template <typename ...Args> requires (LoggerUtil::FormatA<Args>&& ...)
inline void StreamLogger::Critical(std::format_string<Args...> format, Args&& ...args) {
	StreamLogger::Critical(std::format(format, std::forward<Args>(args)...));
}

template <typename ...Args> requires (LoggerUtil::FormatW<Args>&& ...)
inline void StreamLogger::Critical(std::wformat_string<Args...> format, Args&& ...args) {
	StreamLogger::Critical(std::format(format, std::forward<Args>(args)...));
}
