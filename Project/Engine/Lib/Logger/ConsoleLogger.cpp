#include "ConsoleLogger.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <comdef.h>

//* c++
#include <locale>
#ifdef CONSOLE
#include <iostream>
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// ConsoleLogger class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ConsoleLogger::Init() {
	std::locale::global(std::locale("ja_JP.UTF-8")); //!< 日本語出力に対応.
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

void ConsoleLogger::OutputStampA(const std::string_view& label, LoggerUtil::Level level, const Snapshot& snapshot, const std::string_view& message) {
	//!< "<label> [timestamp] [location-filename(one-line)] [thread] [level] message"の形式で出力する.
	ConsoleLogger::OutputA(
		"<{}> [{}] [{}] [{}] [{}] {}",
		label,
		LoggerUtil::SerializeTimestampA(snapshot.GetTimestamp()),
		LoggerUtil::SerializeLocationFilenameOneLineA(snapshot.GetSourceFile(), snapshot.GetSourceLine()),
		LoggerUtil::SerializeThreadstampA(snapshot.GetThreadId()),
		LoggerUtil::SerializeLevelA(level),
		message
	);
}

void ConsoleLogger::OutputStampW(const std::wstring_view& label, LoggerUtil::Level level, const Snapshot& snapshot, const std::wstring_view& message) {
	//!< "<label> [timestamp] [location-filename(one-line)] [thread] [level] message"の形式で出力する.
	ConsoleLogger::OutputW(
		L"<{}> [{}] [{}] [{}] [{}] {}",
		label,
		LoggerUtil::SerializeTimestampW(snapshot.GetTimestamp()),
		LoggerUtil::SerializeLocationFilenameOneLineW(snapshot.GetSourceFile(), snapshot.GetSourceLine()),
		LoggerUtil::SerializeThreadstampW(snapshot.GetThreadId()),
		LoggerUtil::SerializeLevelW(level),
		message
	);
}
