#include "StreamLogger.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <comdef.h>

//* c++
#include <format>
#include <stdexcept>

#ifdef CONSOLE
#include <iostream>
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// StreamLogger class methods
////////////////////////////////////////////////////////////////////////////////////////////

void StreamLogger::Init() {
	if (std::filesystem::exists(kDirectory / kFilename)) LIKELY {
		return; //!< すでに初期化されている場合は何もしない
	}

	{ //!< directoryとfileの作成
		std::filesystem::create_directories(kDirectory);

		std::ofstream file(kDirectory / kFilename, std::ofstream::out | std::ofstream::trunc);
		file << "Sxavenger Library Stream Logger" << "\n";
		file << "profile: " << _PROFILE << "\n";
		file << "main thread id: " << LoggerUtil::kMainThreadId << "\n";
		file << "timestamp: " << LoggerUtil::GetTimestampA(LocalTimePoint::Now()) << "\n";
		file << std::string(100, '#') << "\n";
	}

#ifdef CONSOLE
	{ //!< locateの変更
		std::locale::global(std::locale("ja_JP.UTF-8")); //!< 日本語出力に対応.
	}
#endif

	StreamLogger::Log(LoggerUtil::Level::Info, "StreamLogger | initialize. filename: {}", kFilename.string());
}

void StreamLogger::Log(LoggerUtil::Level level, const std::string_view& message) {
	std::unique_lock<std::mutex> lock(mutex_);
	StreamLogger::OutputStampA(level, message);
}

void StreamLogger::Log(LoggerUtil::Level level, const std::wstring_view& message) {
	std::unique_lock<std::mutex> lock(mutex_);
	StreamLogger::OutputStampW(level, message);
}

NORETURN void StreamLogger::Exception(const std::string_view& message, const TracePoint& point) {

	std::string location  = StreamLogger::GetLocationMessageA(point.location);
	std::string thread    = StreamLogger::GetThreadMessageA(point.id);
	std::string timestamp = StreamLogger::GetTimestampMessageA(point.timestamp);

	{
		std::unique_lock<std::mutex> lock(mutex_);
		//!< 例外発生のログを出力
		StreamLogger::OutputSeparator();

		StreamLogger::OutputA("\n [Critical Error] Sxavenger Stream Logger ExceptionA. \n");

		StreamLogger::OutputA(location);
		StreamLogger::OutputA(thread);
		StreamLogger::OutputA(timestamp);

		StreamLogger::OutputA(message);
		StreamLogger::OutputEndline();

		StreamLogger::OutputSeparator();

		//!< 例外ウィンドウを表示
		StreamLogger::OpenExceptionWindowA(message, location, thread, timestamp);

		StreamLogger::DebugBreak(); //!< 例外ウィンドウを表示した後にデバッガを起動する
	}
}

NORETURN void StreamLogger::Exception(const std::wstring_view& message, const TracePoint& point) {

	std::wstring location  = StreamLogger::GetLocationMessageW(point.location);
	std::wstring thread    = StreamLogger::GetThreadMessageW(point.id);
	std::wstring timestamp = StreamLogger::GetTimestampMessageW(point.timestamp);

	{
		std::unique_lock<std::mutex> lock(mutex_);
		//!< 例外発生のログを出力
		StreamLogger::OutputSeparator();

		StreamLogger::OutputW(L"\n [Critical Error] Sxavenger Stream Logger ExceptionW. \n");

		StreamLogger::OutputW(location);
		StreamLogger::OutputW(thread);
		StreamLogger::OutputW(timestamp);

		StreamLogger::OutputW(message);
		StreamLogger::OutputEndline();

		StreamLogger::OutputSeparator();

		//!< 例外ウィンドウを表示
		StreamLogger::OpenExceptionWindowW(location, thread, timestamp, message);

		StreamLogger::DebugBreak(); //!< 例外ウィンドウを表示した後にデバッガを起動する
	}
}

NORETURN void StreamLogger::Exception(const std::string_view& message, const std::string_view& detail, const TracePoint& point) {
	std::stringstream ss;
	ss << message << "\n\n" << detail;
	StreamLogger::Exception(ss.str(), point);
}

NORETURN void StreamLogger::Exception(const std::wstring_view& message, const std::wstring_view& detail, const TracePoint& point) {
	std::wstringstream ss;
	ss << message << L"\n\n" << detail;
	StreamLogger::Exception(ss.str(), point);
}

void StreamLogger::Assert(bool expression, const std::string_view& message, const TracePoint& point) {
	if (expression) LIKELY {
		 return; //!< assertionがtrueの場合は何もしない
	}

	StreamLogger::Exception(message, point);
}

void StreamLogger::Assert(bool expression, const std::wstring_view& message, const TracePoint& point) {
	if (expression) LIKELY {
		 return; //!< assertionがtrueの場合は何もしない
	}

	StreamLogger::Exception(message, point);
}

void StreamLogger::Assert(bool expression, const std::string_view& message, const std::string_view& detail, const TracePoint& point) {
	if (expression) LIKELY {
		 return; //!< assertionがtrueの場合は何もしない
	}

	StreamLogger::Exception(message, detail, point);
}

void StreamLogger::Assert(bool expression, const std::wstring_view& message, const std::wstring_view& detail, const TracePoint& point) {
	if (expression) LIKELY {
		 return; //!< assertionがtrueの場合は何もしない
	}

	StreamLogger::Exception(message, detail, point);
}

std::string StreamLogger::GetLocationMessageA(const std::source_location& location) {
	std::ostringstream message;
	message << "[location]"  << "\n";
	message << " filename: " << location.file_name()     << "\n";
	message << " function: " << location.function_name() << "\n";
	message << " line:     " << location.line()          << "\n";

	return message.str();
}

std::wstring StreamLogger::GetLocationMessageW(const std::source_location& location) {
	std::wostringstream message;
	message << L"[location]"  << L"\n";
	message << L" filename: " << location.file_name()     << L"\n";
	message << L" function: " << location.function_name() << L"\n";
	message << L" line:     " << location.line()          << L"\n";

	return message.str();
}

std::string StreamLogger::GetThreadMessageA(const std::thread::id id) {
	std::ostringstream message;
	message << "[thread]" << "\n";
	message << " " << LoggerUtil::GetThreadstampA(id) << "\n";
	
	return message.str();
}

std::wstring StreamLogger::GetThreadMessageW(const std::thread::id id) {
	std::wostringstream message;
	message << L"[thread]" << L"\n";
	message << L" " << LoggerUtil::GetThreadstampW(id) << L"\n";
	
	return message.str();
}

std::string StreamLogger::GetTimestampMessageA(const LocalTimePoint& time) {
	std::ostringstream message;
	message << "[timestamp]" << "\n";
	message << " " << LoggerUtil::GetTimestampA(time) << "\n";

	return message.str();
}

std::wstring StreamLogger::GetTimestampMessageW(const LocalTimePoint& time) {
	std::wostringstream message;
	message << L"[timestamp]" << L"\n";
	message << L" " << LoggerUtil::GetTimestampW(time) << L"\n";

	return message.str();
}

void StreamLogger::OutputStampA(LoggerUtil::Level level, const std::string_view& message) {
	//!< timestampとthreadstampの取得
	LocalTimePoint current = LocalTimePoint::Now();
	std::thread::id id     = std::this_thread::get_id();

	OutputA(std::format(
		"[{}] [{}] [{}] {}",
		LoggerUtil::GetTimestampA(current), LoggerUtil::GetThreadstampA(id), LoggerUtil::GetLevelA(level), message
	));
}

void StreamLogger::OutputStampW(LoggerUtil::Level level, const std::wstring_view& message) {
	//!< timestampとthreadstampの取得
	LocalTimePoint current = LocalTimePoint::Now();
	std::thread::id id     = std::this_thread::get_id();

	OutputW(std::format(
		L"[{}] [{}] [{}] {}",
		LoggerUtil::GetTimestampW(current), LoggerUtil::GetThreadstampW(id), LoggerUtil::GetLevelW(level), message
	));
}

void StreamLogger::OutputA(const std::string_view& message) {
	OutputConsoleA(message);
	OutputFileA(message);
}

void StreamLogger::OutputW(const std::wstring_view& message) {
	OutputConsoleW(message);
	OutputFileW(message);
}

void StreamLogger::OutputConsoleA(const std::string_view& message) {
	OutputDebugStringA(message.data());
	OutputDebugStringA("\n");

#ifdef CONSOLE
	std::cout << message << std::endl;
#endif
}

void StreamLogger::OutputConsoleW(const std::wstring_view& message) {
	OutputDebugStringW(message.data());
	OutputDebugStringW(L"\n");

#ifdef CONSOLE
	std::wcout << message << std::endl;
#endif
}

void StreamLogger::OutputFileA(const std::string_view& message) {
	std::ofstream file(kDirectory / kFilename, kModeAppend);
	file << message << "\n";
}

void StreamLogger::OutputFileW(const std::wstring_view& message) {
	std::wofstream file(kDirectory / kFilename, kModeAppend);
	file << message << "\n";
}

void StreamLogger::OutputSeparator(const char separator) {
	const std::string line(100, separator);
	StreamLogger::OutputA(line);
}

void StreamLogger::OutputEndline() {
	StreamLogger::OutputA(""); //!< 空行を出力し, Output側で改行する.
}

NORETURN void StreamLogger::DebugBreak() {
	__debugbreak(); //!< windowsのデバッガを起動する.
	// TODO: ExceptionFilterで呼び出されるようにする.
}

void StreamLogger::OpenExceptionWindowA(
	const std::string_view& message,
	const std::string_view& location,
	const std::string_view& thread,
	const std::string_view& timestamp) {

	std::ostringstream text;
	text << location  << "\n";
	text << thread    << "\n";
	text << timestamp << "\n";
	text << message   << "\n";

	MessageBoxA(
		NULL,
		text.str().c_str(),
		"Sxavenger Stream Logger ExceptionA",
		MB_TASKMODAL | MB_ICONHAND | MB_TOPMOST
	);
}

void StreamLogger::OpenExceptionWindowW(
	const std::wstring_view& location,
	const std::wstring_view& thread,
	const std::wstring_view& timestamp,
	const std::wstring_view& message) {

	std::wostringstream text;
	text << location  << L"\n";
	text << thread    << L"\n";
	text << timestamp << L"\n";
	text << message   << L"\n";

	MessageBoxW(
		NULL,
		text.str().c_str(),
		L"Sxavenger Stream Logger ExceptionW",
		MB_TASKMODAL | MB_ICONHAND | MB_TOPMOST
	);
}
