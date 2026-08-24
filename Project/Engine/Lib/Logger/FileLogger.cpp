#include "FileLogger.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/CXXAttribute.hpp>
#include <Lib/Time/LocalTimePoint.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FileLogger class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FileLogger::Init() {
	if (std::filesystem::exists(kDirectory / kFilename)) LIKELY {
		return; //!< すでに初期化されている場合は何もしない
	}

	{ //!< directoryとfileの作成
		std::filesystem::create_directories(kDirectory);

		std::ofstream file(kDirectory / kFilename, static_cast<std::ofstream::openmode>(Mode::Truncate)); //!< Mode::Truncateで新規作成
		file << "Sxavenger Library File Logger"                                              << '\n';
		file << "profile: "        << _PROFILE                                               << '\n';
		file << "main thread id: " << LoggerUtil::kMainThreadId                              << '\n';
		file << "timestamp: "      << LoggerUtil::SerializeTimestampA(LocalTimePoint::Now()) << '\n';
		file << std::string(100, '#') << '\n';
	}
}

void FileLogger::OutputA(const std::string_view& message) {
	std::unique_lock<std::mutex> lock(mutex_);
	std::ofstream file(kDirectory / kFilename, static_cast<std::ofstream::openmode>(Mode::Append));
	file << message << '\n';
}

void FileLogger::OutputW(const std::wstring_view& message) {
	std::unique_lock<std::mutex> lock(mutex_);
	std::wofstream file(kDirectory / kFilename, static_cast<std::ofstream::openmode>(Mode::Append));
	file << message << '\n';
}

void FileLogger::SeparatorA(const char separator, size_t len) {
	FileLogger::OutputA(std::string(len, separator));
}

void FileLogger::SeparatorW(const wchar_t separator, size_t len) {
	FileLogger::OutputW(std::wstring(len, separator));
}

void FileLogger::Endline() {
	FileLogger::OutputA(""); //!< 空行を出力し, Output側で改行する.
}

void FileLogger::OutputStampA(const std::string_view& label, LoggerUtil::Level level, const TracePoint& point, const std::string_view& message) {
	//!< "<label> [timestamp] [location(one-line)] [thread] [level] message"の形式で出力する.
	FileLogger::OutputA(
		"<{}> [{}] [{}] [{}] [{}] {}",
		label,
		LoggerUtil::SerializeTimestampA(point.timestamp),
		LoggerUtil::SerializeLocationOneLineA(point.location),
		LoggerUtil::SerializeThreadstampA(point.id),
		LoggerUtil::SerializeLevelA(level),
		message
	);
}

void FileLogger::OutputStampW(const std::wstring_view& label, LoggerUtil::Level level, const TracePoint& point, const std::wstring_view& message) {
	//!< "<label> [timestamp] [location(one-line)] [thread] [level] message"の形式で出力する.
	FileLogger::OutputW(
		L"<{}> [{}] [{}] [{}] [{}] {}",
		label,
		LoggerUtil::SerializeTimestampW(point.timestamp),
		LoggerUtil::SerializeLocationOneLineW(point.location),
		LoggerUtil::SerializeThreadstampW(point.id),
		LoggerUtil::SerializeLevelW(level),
		message
	);
}

std::filesystem::path FileLogger::CreateFilename(const std::filesystem::path& extension) {

	const LocalTimePoint current = LocalTimePoint::Now();

	const LocalTimePoint::Date& date = current.date;
	const LocalTimePoint::Time& time = current.time;

	//!< "YYYY-MM-DD_hh-mm-ss"形式
	return std::format(
		"{:04}-{:02}-{:02}_{:02}-{:02}-{:02}{}",
		date.year, date.month, date.day, time.hour, time.minute, time.second, extension.string()
	);
	
}
