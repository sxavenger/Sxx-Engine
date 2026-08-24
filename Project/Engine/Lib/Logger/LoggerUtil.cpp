#include "LoggerUtil.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <sstream>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// LoggerUtil namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string_view LoggerUtil::SerializeLevelA(Level level) noexcept {
	switch (level) {
		case Level::Debug:    return "Debug";
		case Level::Info:     return "Info";
		case Level::Warning:  return "Warning";
		case Level::Error:    return "Error";
		case Level::Critical: return "Critical";
		default:              return "Unknown";
	};
}

std::wstring_view LoggerUtil::SerializeLevelW(Level level) noexcept {
	switch (level) {
		case Level::Debug:    return L"Debug";
		case Level::Info:     return L"Info";
		case Level::Warning:  return L"Warning";
		case Level::Error:    return L"Error";
		case Level::Critical: return L"Critical";
		default:              return L"Unknown";
	};
}

std::string LoggerUtil::SerializeTimestampA(const LocalTimePoint& time) {
	return time.SerializeA();
}

std::wstring LoggerUtil::SerializeTimestampW(const LocalTimePoint& time) {
	return time.SerializeW();
}

std::string LoggerUtil::SerializeThreadstampA(std::thread::id id) {
	std::ostringstream stamp;

	if (id == LoggerUtil::kMainThreadId) {
		stamp << "main thread";

	} else {
		stamp << "thread id: " << id;
	}

	return stamp.str();
}

std::wstring LoggerUtil::SerializeThreadstampW(std::thread::id id) {
	std::wostringstream stamp;

	if (id == LoggerUtil::kMainThreadId) {
		stamp << L"main thread";

	} else {
		stamp << L"thread id: " << id;
	}

	return stamp.str();
}

std::string LoggerUtil::SerializeLocationOneLineA(const std::source_location& location) {
	return std::format("{}:{}", std::filesystem::path(location.file_name()).filename().string(), location.line());
}

std::wstring LoggerUtil::SerializeLocationOneLineW(const std::source_location& location) {
	return std::format(L"{}:{}", std::filesystem::path(location.file_name()).filename().wstring(), location.line());
}
