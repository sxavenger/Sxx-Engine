#include "ConsoleLogger.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <comdef.h>

//* c++
#ifdef CONSOLE
#include <iostream>
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// ConsoleLogger class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ConsoleLogger::Init() {
#ifdef CONSOLE
	std::locale::global(std::locale("ja_JP.UTF-8")); //!< 日本語出力に対応.
#endif
}

void ConsoleLogger::OutputA(const std::string_view& message) {
	std::unique_lock<std::mutex> lock(mutex_);

	//!< console出力
#ifdef CONSOLE
	std::cout << message << std::endl;
#endif

	//!< debug出力
	OutputDebugStringA(message.data());
	OutputDebugStringA("\n");
}

void ConsoleLogger::OutputW(const std::wstring_view& message) {
	std::unique_lock<std::mutex> lock(mutex_);

	//!< console出力
#ifdef CONSOLE
	std::wcout << message << std::endl;
#endif

	//!< debug出力
	OutputDebugStringW(message.data());
	OutputDebugStringW(L"\n");
}

void ConsoleLogger::SeparatorA(const char separator, size_t len) {
	ConsoleLogger::OutputA(std::string(len, separator));
}

void ConsoleLogger::SeparatorW(const wchar_t separator, size_t len) {
	ConsoleLogger::OutputW(std::wstring(len, separator));
}

void ConsoleLogger::Endline() {
	ConsoleLogger::OutputA(""); //!< 空行を出力し, Output側で改行する.
}

void ConsoleLogger::OutputStampA(const std::string_view& label, LoggerUtil::Level level, const TracePoint& point, const std::string_view& message) {
	//!< "<label> [timestamp] [location(one-line)] [thread] [level] message"の形式で出力する.
	ConsoleLogger::OutputA(
		"<{}> [{}] [{}] [{}] [{}] {}",
		label,
		LoggerUtil::SerializeTimestampA(point.timestamp),
		LoggerUtil::SerializeLocationOneLineA(point.location),
		LoggerUtil::SerializeThreadstampA(point.id),
		LoggerUtil::SerializeLevelA(level),
		message
	);
}

void ConsoleLogger::OutputStampW(const std::wstring_view& label, LoggerUtil::Level level, const TracePoint& point, const std::wstring_view& message) {
	//!< "<label> [timestamp] [location(one-line)] [thread] [level] message"の形式で出力する.
	ConsoleLogger::OutputW(
		L"<{}> [{}] [{}] [{}] [{}] {}",
		label,
		LoggerUtil::SerializeTimestampW(point.timestamp),
		LoggerUtil::SerializeLocationOneLineW(point.location),
		LoggerUtil::SerializeThreadstampW(point.id),
		LoggerUtil::SerializeLevelW(level),
		message
	);
}
