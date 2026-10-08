#include "StackLogger.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "ConsoleLogger.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Stamp structure methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string StackLogger::Stamp::SerializeA() const {
	return std::format(
		"[{}] [{}] [{}] [{}] {} [x{}]",
		LoggerUtil::SerializeTimestampA(point.timestamp),
		LoggerUtil::SerializeLocationOneLineA(point.location),
		LoggerUtil::SerializeThreadstampA(point.id),
		LoggerUtil::SerializeLevelA(level),
		message,
		count
	);
}

bool StackLogger::Stamp::Equals(const Stamp& lhs, const Stamp& rhs) noexcept {
	return
		lhs.level == rhs.level &&
		lhs.point.id == rhs.point.id &&
		lhs.point.location.file_name() == rhs.point.location.file_name() &&
		lhs.point.location.function_name() == rhs.point.location.function_name() &&
		lhs.point.location.line() == rhs.point.location.line() &&
		lhs.message == rhs.message;
}

////////////////////////////////////////////////////////////////////////////////////////////
// StackLogger class methods
////////////////////////////////////////////////////////////////////////////////////////////

void StackLogger::StackStampA(LoggerUtil::Level level, const TracePoint& point, const std::string_view& message) {

	Stamp stamp = {};
	stamp.level   = level;
	stamp.point   = point;
	stamp.message = std::string(message);
	stamp.count   = 1;

	std::unique_lock<std::mutex> lock(mutex_);

	if (!stamps_.empty() && Stamp::Equals(stamps_.back().first, stamp)) {
		//!< スタックの最後のLogと同じ場合はcountを増やす

		Stamp& last = stamps_.back().first;
		last.count++;
		last.point.timestamp = point.timestamp; //!< タイムスタンプを更新する

		stamps_.back().second = last.SerializeA(); //!< シリアライズ文字列を更新する
		return;
	}

	stamps_.emplace_back(std::move(stamp), stamp.SerializeA());
	ConsoleLogger::OutputStampA("Stack", level, point, message);

	//!< スタックの最大件数を超えた場合は古いLogを削除する
	while (stamps_.size() > kMaxStampCount) {
		stamps_.pop_front();
	}
}

void StackLogger::Log(LoggerUtil::Level level, const TracePoint& point, const std::string_view& message) {
	StackLogger::StackStampA(level, point, message);
}
