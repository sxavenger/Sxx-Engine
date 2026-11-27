#include "StackLogger.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "ConsoleLogger.h"
#include "FileLogger.h"

////////////////////////////////////////////////////////////////////////////////////////////
// [StackLogger] Stamp structure methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string StackLogger::Stamp::SerializeA() const {
	//!< [first-timestamp ~ last-timestamp] [location] [thread] [level] message [xcount]
	return std::format(
		"[{} ~ {}] [{}] [{}] [{}] {} [x{}]",
		LoggerUtil::SerializeTimestampA(firstTimestamp),
		LoggerUtil::SerializeTimestampA(lastTimestamp),
		LoggerUtil::SerializeLocationOneLineA(filename, line),
		LoggerUtil::SerializeThreadstampA(id),
		LoggerUtil::SerializeLevelA(level),
		message,
		count
	);
}

bool StackLogger::Stamp::Equals(const Stamp& lhs, const Stamp& rhs) noexcept {
	return
		lhs.level    == rhs.level &&
		lhs.id       == rhs.id &&
		lhs.filename == rhs.filename &&
		lhs.line     == rhs.line &&
		lhs.message  == rhs.message;
}

////////////////////////////////////////////////////////////////////////////////////////////
// StackLogger class methods
////////////////////////////////////////////////////////////////////////////////////////////

void StackLogger::StackStampA(LoggerUtil::Level level, const Snapshot& snapshot, const std::string_view& message) {

	Stamp stamp = {};
	stamp.level          = level;
	stamp.id             = snapshot.GetThreadId();
	stamp.firstTimestamp = snapshot.GetTimestamp();
	stamp.lastTimestamp  = snapshot.GetTimestamp();
	stamp.filename       = snapshot.GetSourceFile().filename().string();
	stamp.line           = snapshot.GetSourceLine();
	stamp.message        = std::string(message);
	stamp.count          = 1;

	std::unique_lock<std::mutex> lock(mutex_);

	FileLogger::OutputStampA("Stack", level, snapshot, message);

	if (!stamps_.empty() && Stamp::Equals(stamps_.back().first, stamp)) {
		//!< スタックの最後のLogと同じ場合はcountを増やす

		Stamp& last = stamps_.back().first;
		last.count++;
		last.lastTimestamp = snapshot.GetTimestamp(); //!< 最終タイムスタンプを更新する

		stamps_.back().second = last.SerializeA(); //!< シリアライズ文字列を更新する
		return;
	}

	stamps_.emplace_back(std::move(stamp), stamp.SerializeA());
	ConsoleLogger::OutputStampA("Stack", level, snapshot, message);

	//!< スタックの最大件数を超えた場合は古いLogを削除する
	while (stamps_.size() > kMaxStampCount) {
		stamps_.pop_front();
	}
}

void StackLogger::Log(LoggerUtil::Level level, const Snapshot& snapshot, const std::string_view& message) {
	StackLogger::StackStampA(level, snapshot, message);
}
