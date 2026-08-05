#include "LoggerUtil.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// LoggerUtil namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string LoggerUtil::GetTimestampA(const LocalTimePoint& time) {
	return time.SerializeA();
}

std::wstring LoggerUtil::GetTimestampW(const LocalTimePoint& time) {
	return time.SerializeW();
}

std::string LoggerUtil::GetThreadstampA(const std::thread::id& id) {
	std::ostringstream stamp;

	if (id == LoggerUtil::kMainThreadId) {
		stamp << "main thread";
	} else {
		stamp << "thread id: " << id;
	}

	return stamp.str();
}

std::wstring LoggerUtil::GetThreadstampW(const std::thread::id& id) {
	std::wostringstream stamp;

	if (id == LoggerUtil::kMainThreadId) {
		stamp << L"main thread";

	} else {
		stamp << L"thread id: " << id;
	}

	return stamp.str();
}

std::string LoggerUtil::GetLevelA(Level level) {
	switch (level) {
		case Level::Debug:    return "Debug";
		case Level::Info:     return "Info";
		case Level::Warning:  return "Warning";
		case Level::Error:    return "Error";
		case Level::Critical: return "Critical";
		default:              return "Unknown";
	};
}

std::wstring LoggerUtil::GetLevelW(Level level) {
	switch (level) {
		case Level::Debug:    return L"Debug";
		case Level::Info:     return L"Info";
		case Level::Warning:  return L"Warning";
		case Level::Error:    return L"Error";
		case Level::Critical: return L"Critical";
		default:              return L"Unknown";
	};
}

std::filesystem::path LoggerUtil::CreateFilename(const std::filesystem::path& extension) {

	const LocalTimePoint current = LocalTimePoint::Now();

	const LocalTimePoint::Date& date = current.date;
	const LocalTimePoint::Time& time = current.time;

	//!< "YYYY-MM-DD_hh-mm-ss"形式
	return std::format(
		"{:04}-{:02}-{:02}_{:02}-{:02}-{:02}{}",
		date.year, date.month, date.day, time.hour, time.minute, time.second, extension.string()
	);
	
}
