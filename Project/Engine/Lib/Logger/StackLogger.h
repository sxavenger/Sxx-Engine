#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "LoggerUtil.h"

//* c++
#include <mutex>
#include <deque>

////////////////////////////////////////////////////////////////////////////////////////////
// StackLogger class
////////////////////////////////////////////////////////////////////////////////////////////
class StackLogger final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Stamp structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Stamp final {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* serialize methods *//

		std::string SerializeA() const;

		//* compare methods *//

		static bool Equals(const Stamp& lhs, const Stamp& rhs) noexcept;

		//=========================================================================================
		// public variables
		//=========================================================================================

		LoggerUtil::Level level;
		TracePoint point;
		std::string message;
		size_t count;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief スタックにLogを追加する. 同じLogが連続して追加された場合はcountを増やす.
	static void StackStampA(LoggerUtil::Level level, const TracePoint& point, const std::string_view& message);

	// TODO: StackStampWを実装する場合は、messageをstd::wstringに変更する必要がある.

	//* log override methods *//

	static void Log(LoggerUtil::Level level, const TracePoint& point, const std::string_view& message);

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	static void Log(LoggerUtil::Level level, const TracePoint& point, std::format_string<Args...> format, Args&&... args);

	//* stack log methods *//

	static const std::deque<std::pair<Stamp, std::string>>& GetStamps() { return stamps_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline std::mutex mutex_;
	static inline std::deque<std::pair<Stamp, std::string>> stamps_;
	
	constexpr static inline const size_t kMaxStampCount = 16384; //!< スタックに保持する最大件数.

};

////////////////////////////////////////////////////////////////////////////////////////////
// StackLogger class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
inline void StackLogger::Log(LoggerUtil::Level level, const TracePoint& point, std::format_string<Args...> format, Args&&... args) {
	StackLogger::StackStampA(level, point, std::format(format, std::forward<Args>(args)...));
}

//-----------------------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------------------

// Logging macros

#define STACK_LOG_DEBUG(format, ...)    StackLogger::Log(LoggerUtil::Level::Debug, TracePoint::Current(), format, ##__VA_ARGS__)
#define STACK_LOG_INFO(format, ...)     StackLogger::Log(LoggerUtil::Level::Info, TracePoint::Current(), format, ##__VA_ARGS__)
#define STACK_LOG_WARNING(format, ...)  StackLogger::Log(LoggerUtil::Level::Warning, TracePoint::Current(), format, ##__VA_ARGS__)
#define STACK_LOG_ERROR(format, ...)    StackLogger::Log(LoggerUtil::Level::Error, TracePoint::Current(), format, ##__VA_ARGS__)
#define STACK_LOG_CRITICAL(format, ...) StackLogger::Log(LoggerUtil::Level::Critical, TracePoint::Current(), format, ##__VA_ARGS__)
