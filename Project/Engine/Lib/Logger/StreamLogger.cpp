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

void StreamLogger::OutputStampA(LoggerUtil::Level level, const Snapshot& snapshot, const std::string_view& message) {
	std::unique_lock<std::mutex> lock(mutex_);
	ConsoleLogger::OutputStampA("Stream", level, snapshot, message);
	FileLogger::OutputStampA("Stream", level, snapshot, message);
}

void StreamLogger::OutputStampW(LoggerUtil::Level level, const Snapshot& snapshot, const std::wstring_view& message) {
	std::unique_lock<std::mutex> lock(mutex_);
	ConsoleLogger::OutputStampW(L"Stream", level, snapshot, message);
	FileLogger::OutputStampW(L"Stream", level, snapshot, message);
}

void StreamLogger::Log(LoggerUtil::Level level, const Snapshot& snapshot, const std::string_view& message) {
	StreamLogger::OutputStampA(level, snapshot, message);
}

void StreamLogger::Log(LoggerUtil::Level level, const Snapshot& snapshot, const std::wstring_view& message) {
	StreamLogger::OutputStampW(level, snapshot, message);
}

NORETURN void StreamLogger::ExceptionA(const SnapshotDetailed& snapshot, const std::string_view& message) {

	std::string location   = StreamLogger::GetLocationMessageA(snapshot.GetSourceFile(), snapshot.GetSourceFunction(), snapshot.GetSourceLine());
	std::string thread     = StreamLogger::GetThreadMessageA(snapshot.GetThreadId());
	std::string timestamp  = StreamLogger::GetTimestampMessageA(snapshot.GetTimestamp());
	std::string stacktrace = StreamLogger::GetStacktraceMessageA(snapshot.GetStacktrace());

	{
		std::unique_lock<std::mutex> lock(mutex_);

		{ //!< 例外発生のログを出力
			StreamLogger::SeparatorA('=');

			StreamLogger::OutputA("\n [Exception] Sxavenger Stream Logger Exception. \n");

			StreamLogger::OutputA(location);
			StreamLogger::OutputA(thread);
			StreamLogger::OutputA(timestamp);
			StreamLogger::OutputA(stacktrace);

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

NORETURN void StreamLogger::ExceptionW(const SnapshotDetailed& snapshot, const std::wstring_view& message) {

	std::wstring location  = StreamLogger::GetLocationMessageW(snapshot.GetSourceFile(), snapshot.GetSourceFunction(), snapshot.GetSourceLine());
	std::wstring thread    = StreamLogger::GetThreadMessageW(snapshot.GetThreadId());
	std::wstring timestamp = StreamLogger::GetTimestampMessageW(snapshot.GetTimestamp());
	std::wstring stacktrace = StreamLogger::GetStacktraceMessageW(snapshot.GetStacktrace());

	{
		std::unique_lock<std::mutex> lock(mutex_);

		{ //!< 例外発生のログを出力
			StreamLogger::SeparatorW(L'=');

			StreamLogger::OutputW(L"\n [Exception] Sxavenger Stream Logger Exception. \n");

			StreamLogger::OutputW(location);
			StreamLogger::OutputW(thread);
			StreamLogger::OutputW(timestamp);
			StreamLogger::OutputW(stacktrace);

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

NORETURN void StreamLogger::ExceptionSummaryA(const SnapshotDetailed& snapshot, const std::string_view& summary, const std::string_view& message) {
	std::stringstream stream;
	stream << summary << "\n\n" << message;
	StreamLogger::ExceptionA(snapshot, stream.str());
}

NORETURN void StreamLogger::ExceptionSummaryW(const SnapshotDetailed& snapshot, const std::wstring_view& summary, const std::wstring_view& message) {
	std::wstringstream stream;
	stream << summary << L"\n\n" << message;
	StreamLogger::ExceptionW(snapshot, stream.str());
}

NORETURN void StreamLogger::Exception(const SnapshotDetailed& snapshot, const std::string_view& message) {
	StreamLogger::ExceptionA(snapshot, message);
}

NORETURN void StreamLogger::Exception(const SnapshotDetailed& snapshot, const std::wstring_view& message) {
	StreamLogger::ExceptionW(snapshot, message);
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

std::string StreamLogger::GetLocationMessageA(const std::filesystem::path& file, const std::string_view& function, const uint_least32_t line) {
	std::ostringstream message;
	message << "[location]"  << "\n";
	message << " filepath: " << file.generic_string() << "\n";
	message << " function: " << function              << "\n";
	message << " line:     " << line                  << "\n";

	return message.str();
}

std::wstring StreamLogger::GetLocationMessageW(const std::filesystem::path& file, const std::string_view& function, const uint_least32_t line) {
	std::wostringstream message;
	message << L"[location]"  << L"\n";
	message << L" filepath: " << file.generic_wstring() << L"\n";
	message << L" function: " << function.data()        << L"\n";
	message << L" line:     " << line                   << L"\n";

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

std::string StreamLogger::GetStacktraceMessageA(const std::span<const SnapshotDetailed::StackFrame>& stacktrace) {

	const size_t count = std::min<size_t>(stacktrace.size(), 8); //!< 表示するスタックフレームの最大数を8に制限する.

	std::ostringstream message;
	message << "[stacktrace]" << "\n";

	for (size_t i = 0; i < count; ++i) {
		const SnapshotDetailed::StackFrame& frame = stacktrace[i];
		//!< (i# description | filepath:line)の形式で出力する.
		message << " " << i << "# " << frame.description << " | " << frame.file.generic_string() << ":" << frame.line << "\n";
	}

	if (stacktrace.size() > count) {
		message << " ... (more " << (stacktrace.size() - count) << " frames)" << "\n"; //!< スタックフレームが多すぎる場合は省略して表示する.
	}

	return message.str();	
}

std::wstring StreamLogger::GetStacktraceMessageW(const std::span<const SnapshotDetailed::StackFrame>& stacktrace) {

	const size_t count = std::min<size_t>(stacktrace.size(), 8); //!< 表示するスタックフレームの最大数を8に制限する.

	std::wostringstream message;
	message << L"[stacktrace]" << L"\n";

	for (size_t i = 0; i < count; ++i) {
		const SnapshotDetailed::StackFrame& frame = stacktrace[i];
		//!< (i# description | filepath:line)の形式で出力する.
		message << " " << i << L"# " << frame.description.data() << L" | " << frame.file.generic_wstring() << L":" << frame.line << L"\n";
	}

	if (stacktrace.size() > count) {
		message << " ... (more " << (stacktrace.size() - count) << " frames)" << L"\n"; //!< スタックフレームが多すぎる場合は省略して表示する.
	}

	return message.str();
}
