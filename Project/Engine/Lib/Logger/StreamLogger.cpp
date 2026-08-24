#include "StreamLogger.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "ConsoleLogger.h"
#include "FileLogger.h"
#include "MessageDialog.h"
#include "CrashHandler.h"

////////////////////////////////////////////////////////////////////////////////////////////
// StreamLogger class methods
////////////////////////////////////////////////////////////////////////////////////////////

void StreamLogger::OutputStampA(LoggerUtil::Level level, const TracePoint& point, const std::string_view& message) {
	std::unique_lock<std::mutex> lock(mutex_);
	ConsoleLogger::OutputStampA("Stream", level, point, message);
	FileLogger::OutputStampA("Stream", level, point, message);
}

void StreamLogger::OutputStampW(LoggerUtil::Level level, const TracePoint& point, const std::wstring_view& message) {
	std::unique_lock<std::mutex> lock(mutex_);
	ConsoleLogger::OutputStampW(L"Stream", level, point, message);
	FileLogger::OutputStampW(L"Stream", level, point, message);
}

void StreamLogger::Log(LoggerUtil::Level level, const TracePoint& point, const std::string_view& message) {
	StreamLogger::OutputStampA(level, point, message);
}

void StreamLogger::Log(LoggerUtil::Level level, const TracePoint& point, const std::wstring_view& message) {
	StreamLogger::OutputStampW(level, point, message);
}

NORETURN void StreamLogger::ExceptionA(const TracePoint& point, const std::string_view& message) {

	std::string location  = StreamLogger::GetLocationMessageA(point.location);
	std::string thread    = StreamLogger::GetThreadMessageA(point.id);
	std::string timestamp = StreamLogger::GetTimestampMessageA(point.timestamp);

	{
		std::unique_lock<std::mutex> lock(mutex_);

		{ //!< 例外発生のログを出力
			StreamLogger::SeparatorA('=');

			StreamLogger::OutputA("\n [Exception] Sxavenger Stream Logger Exception. \n");

			StreamLogger::OutputA(location);
			StreamLogger::OutputA(thread);
			StreamLogger::OutputA(timestamp);

			StreamLogger::OutputA(message);
			StreamLogger::Endline();

			StreamLogger::SeparatorA('=');
		}
		
		//!< 例外ウィンドウを表示
		{
			std::ostringstream text;
			text << location  << '\n';
			text << thread    << '\n';
			text << timestamp << '\n';
			text << message   << '\n';

			MessageDialog::ShowA("Sxavenger Stream Logger Exception", text.str(), MessageDialog::Icon::Error);
		}

		//!< 例外ウィンドウを表示した後にデバッガを起動する
		CrashHandler::Breakpoint();
	}
}

NORETURN void StreamLogger::ExceptionW(const TracePoint& point, const std::wstring_view& message) {

	std::wstring location  = StreamLogger::GetLocationMessageW(point.location);
	std::wstring thread    = StreamLogger::GetThreadMessageW(point.id);
	std::wstring timestamp = StreamLogger::GetTimestampMessageW(point.timestamp);

	{
		std::unique_lock<std::mutex> lock(mutex_);

		{ //!< 例外発生のログを出力
			StreamLogger::SeparatorW(L'=');
			StreamLogger::OutputW(L"\n [Exception] Sxavenger Stream Logger Exception. \n");
			StreamLogger::OutputW(location);
			StreamLogger::OutputW(thread);
			StreamLogger::OutputW(timestamp);
			StreamLogger::OutputW(message);
			StreamLogger::Endline();
			StreamLogger::SeparatorW(L'=');
		}
		
		//!< 例外ウィンドウを表示
		{
			std::wostringstream text;
			text << location  << L'\n';
			text << thread    << L'\n';
			text << timestamp << L'\n';
			text << message   << L'\n';

			MessageDialog::ShowW(L"Sxavenger Stream Logger Exception", text.str(), MessageDialog::Icon::Error);
		}

		//!< 例外ウィンドウを表示した後にデバッガを起動する
		CrashHandler::Breakpoint();
	}
}

NORETURN void StreamLogger::ExceptionSummaryA(const TracePoint& point, const std::string_view& summary, const std::string_view& message) {
	std::stringstream stream;
	stream << summary << "\n\n" << message;
	StreamLogger::ExceptionA(point, stream.str());
}

NORETURN void StreamLogger::ExceptionSummaryW(const TracePoint& point, const std::wstring_view& summary, const std::wstring_view& message) {
	std::wstringstream stream;
	stream << summary << L"\n\n" << message;
	StreamLogger::ExceptionW(point, stream.str());
}

NORETURN void StreamLogger::Exception(const TracePoint& point, const std::string_view& message) {
	StreamLogger::ExceptionA(point, message);
}

NORETURN void StreamLogger::Exception(const TracePoint& point, const std::wstring_view& message) {
	StreamLogger::ExceptionW(point, message);
}

void StreamLogger::AssertA(bool expression, const TracePoint& point, const std::string_view& message) {
	if (expression) LIKELY {
		 return; //!< assertionがtrueの場合は何もしない
	}

	StreamLogger::ExceptionA(point, message);
}

void StreamLogger::AssertW(bool expression, const TracePoint& point, const std::wstring_view& message) {
	if (expression) LIKELY {
		 return; //!< assertionがtrueの場合は何もしない
	}

	StreamLogger::ExceptionW(point, message);
}

void StreamLogger::AssertSummaryA(bool expression, const TracePoint& point, const std::string_view& summary, const std::string_view& message) {
	if (expression) LIKELY {
		 return; //!< assertionがtrueの場合は何もしない
	}

	StreamLogger::ExceptionSummaryA(point, summary, message);
}

void StreamLogger::AssertSummaryW(bool expression, const TracePoint& point, const std::wstring_view& summary, const std::wstring_view& message) {
	if (expression) LIKELY {
		 return; //!< assertionがtrueの場合は何もしない
	}

	StreamLogger::ExceptionSummaryW(point, summary, message);
}

void StreamLogger::Assert(bool expression, const TracePoint& point, const std::string_view& message) {
	StreamLogger::AssertA(expression, point, message);
}

void StreamLogger::Assert(bool expression, const TracePoint& point, const std::wstring_view& message) {
	StreamLogger::AssertW(expression, point, message);
}

void StreamLogger::AssertSummary(bool expression, const TracePoint& point, const std::string_view& summary, const std::string_view& message) {
	StreamLogger::AssertSummaryA(expression, point, summary, message);
}

void StreamLogger::AssertSummary(bool expression, const TracePoint& point, const std::wstring_view& summary, const std::wstring_view& message) {
	StreamLogger::AssertSummaryW(expression, point, summary, message);
}

void StreamLogger::OutputA(const std::string_view& message) {
	ConsoleLogger::OutputA(message);
	FileLogger::OutputA(message);
}

void StreamLogger::OutputW(const std::wstring_view& message) {
	ConsoleLogger::OutputW(message);
	FileLogger::OutputW(message);
}

void StreamLogger::SeparatorA(const char separator, size_t len) {
	ConsoleLogger::SeparatorA(separator, len);
	FileLogger::SeparatorA(separator, len);
}

void StreamLogger::SeparatorW(const wchar_t separator, size_t len) {
	ConsoleLogger::SeparatorW(separator, len);
	FileLogger::SeparatorW(separator, len);
}

void StreamLogger::Endline() {
	ConsoleLogger::Endline();
	FileLogger::Endline();
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
	message << " " << LoggerUtil::SerializeThreadstampA(id) << "\n";
	
	return message.str();
}

std::wstring StreamLogger::GetThreadMessageW(const std::thread::id id) {
	std::wostringstream message;
	message << L"[thread]" << L"\n";
	message << L" " << LoggerUtil::SerializeThreadstampW(id) << L"\n";
	
	return message.str();
}

std::string StreamLogger::GetTimestampMessageA(const LocalTimePoint& time) {
	std::ostringstream message;
	message << "[timestamp]" << "\n";
	message << " " << LoggerUtil::SerializeTimestampA(time) << "\n";

	return message.str();
}

std::wstring StreamLogger::GetTimestampMessageW(const LocalTimePoint& time) {
	std::wostringstream message;
	message << L"[timestamp]" << L"\n";
	message << L" " << LoggerUtil::SerializeTimestampW(time) << L"\n";

	return message.str();
}
