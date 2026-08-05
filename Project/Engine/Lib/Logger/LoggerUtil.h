#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include "../Time/LocalTimePoint.h"

//* c++
#include <cstdint>
#include <filesystem>
#include <thread>
#include <string>
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// LoggerUtil namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace LoggerUtil {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Level enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Level : uint8_t {
		Debug,
		Info,
		Warning,
		Error,
		Critical
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//* message serialization helper methods *//

	std::string GetTimestampA(const LocalTimePoint& time);
	std::wstring GetTimestampW(const LocalTimePoint& time);

	std::string GetThreadstampA(const std::thread::id& id);
	std::wstring GetThreadstampW(const std::thread::id& id);

	std::string GetLevelA(Level level);
	std::wstring GetLevelW(Level level);
	
	//* file system methods *//

	std::filesystem::path CreateFilename(const std::filesystem::path& extension);
	//!< "YYYY-MM-DD_hh-mm-ss"形式を命名規則とする.

	////////////////////////////////////////////////////////////////////////////////////////////
	// constant variables
	////////////////////////////////////////////////////////////////////////////////////////////

	//* thread *//

	static inline const std::thread::id kMainThreadId = std::this_thread::get_id();

	////////////////////////////////////////////////////////////////////////////////////////////
	// concepts
	////////////////////////////////////////////////////////////////////////////////////////////

	template <typename T>
	concept FormatA = requires(T value, std::format_context& ctx) {
		typename std::formatter<std::remove_cvref_t<T>, char>;
		std::formatter<std::remove_cvref_t<T>, char>{}.format(value, ctx);
	};

	template <typename T>
	concept FormatW = requires(T value, std::wformat_context& ctx) {
		typename std::formatter<std::remove_cvref_t<T>, wchar_t>;
		std::formatter<std::remove_cvref_t<T>, wchar_t>{}.format(value, ctx);
	};

}
