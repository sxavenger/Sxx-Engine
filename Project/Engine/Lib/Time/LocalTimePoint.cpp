#include "LocalTimePoint.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// [LocalTimePoint] Date structure methods
////////////////////////////////////////////////////////////////////////////////////////////

LocalTimePoint::Date::Date(const std::chrono::year_month_day& ymd) noexcept
	: year(static_cast<uint16_t>(int(ymd.year()))),
	month(static_cast<uint8_t>(unsigned(ymd.month()))),
	day(static_cast<uint8_t>(unsigned(ymd.day()))) {
}

LocalTimePoint::Date& LocalTimePoint::Date::operator=(const std::chrono::year_month_day& ymd) noexcept {
	year   = static_cast<uint16_t>(int(ymd.year()));
	month = static_cast<uint8_t>(unsigned(ymd.month()));
	day   = static_cast<uint8_t>(unsigned(ymd.day()));

	return *this;
}

bool LocalTimePoint::Date::operator==(const Date& other) const noexcept {
	return year == other.year && month == other.month && day == other.day;
}

 bool LocalTimePoint::Date::operator!=(const Date& other) const noexcept {
	return year != other.year || month != other.month || day != other.day;
}

std::string LocalTimePoint::Date::SerializeA() const {
	return std::format("{:04}-{:02}-{:02}", year, month, day); //!< "YYYY-MM-DD" 形式で返却
}

std::wstring LocalTimePoint::Date::SerializeW() const {
	return std::format(L"{:04}-{:02}-{:02}", year, month, day); //!< "YYYY-MM-DD" 形式で返却
}

LocalTimePoint::Date LocalTimePoint::Date::Deserialize(const std::string& s) {
	Date date = {};

	//!< 文字列の形式は "YYYY-MM-DD"
	date.year  = static_cast<uint16_t>(std::stoi(s.substr(0, 4))); //!< [0000-9999]
	date.month = static_cast<uint8_t>(std::stoi(s.substr(5, 2)));  //!< [01-12]
	date.day   = static_cast<uint8_t>(std::stoi(s.substr(8, 2)));  //!< [01-31]

	return date;
}

////////////////////////////////////////////////////////////////////////////////////////////
// [LocalTimePoint] Time structure methods
////////////////////////////////////////////////////////////////////////////////////////////

LocalTimePoint::Time::Time(const std::chrono::hh_mm_ss<std::chrono::seconds>& hms) noexcept
	: hour(static_cast<uint8_t>(hms.hours().count())),
	minute(static_cast<uint8_t>(hms.minutes().count())),
	second(static_cast<uint8_t>(hms.seconds().count())) {
}

LocalTimePoint::Time& LocalTimePoint::Time::operator=(const std::chrono::hh_mm_ss<std::chrono::seconds>& hms) noexcept {
	hour   = static_cast<uint8_t>(hms.hours().count());
	minute = static_cast<uint8_t>(hms.minutes().count());
	second = static_cast<uint8_t>(hms.seconds().count());

	return *this;
}

bool LocalTimePoint::Time::operator==(const Time& other) const noexcept {
	return hour == other.hour && minute == other.minute && second == other.second;
}

bool LocalTimePoint::Time::operator!=(const Time& other) const noexcept {
	return hour != other.hour || minute != other.minute || second != other.second;
}

std::string LocalTimePoint::Time::SerializeA() const {
	return std::format("{:02}:{:02}:{:02}", hour, minute, second); //!< "HH:MM:SS" 形式で返却
}

std::wstring LocalTimePoint::Time::SerializeW() const {
	return std::format(L"{:02}:{:02}:{:02}", hour, minute, second); //!< "HH:MM:SS" 形式で返却
}

LocalTimePoint::Time LocalTimePoint::Time::Deserialize(const std::string& s) {
	Time time = {};

	//!< 文字列の形式は "HH:MM:SS"
	time.hour   = static_cast<uint8_t>(std::stoi(s.substr(0, 2))); //!< [00-23]
	time.minute = static_cast<uint8_t>(std::stoi(s.substr(3, 2))); //!< [00-59]
	time.second = static_cast<uint8_t>(std::stoi(s.substr(6, 2))); //!< [00-59]

	return time;
}

////////////////////////////////////////////////////////////////////////////////////////////
// LocalTimePoint structure methods
////////////////////////////////////////////////////////////////////////////////////////////

LocalTimePoint::LocalTimePoint(const std::chrono::zoned_time<std::chrono::seconds>& zone) noexcept {
	std::chrono::local_time<std::chrono::seconds> local = zone.get_local_time();
	std::chrono::local_time<std::chrono::days> day      = std::chrono::floor<std::chrono::days>(local);

	date = std::chrono::year_month_day(day);
	time = std::chrono::hh_mm_ss{ local - day };
}

LocalTimePoint& LocalTimePoint::operator=(const std::chrono::zoned_time<std::chrono::seconds>& zone) noexcept {
	std::chrono::local_time<std::chrono::seconds> local = zone.get_local_time();
	std::chrono::local_time<std::chrono::days> day      = std::chrono::floor<std::chrono::days>(local);

	date = std::chrono::year_month_day(day);
	time = std::chrono::hh_mm_ss{ local - day };

	return *this;
}

bool LocalTimePoint::operator==(const LocalTimePoint& other) const noexcept {
	return date == other.date && time == other.time;
}

bool LocalTimePoint::operator!=(const LocalTimePoint& other) const noexcept {
	return date != other.date || time != other.time;
}

std::string LocalTimePoint::SerializeA() const {
	return std::format("{} {}", date.SerializeA(), time.SerializeA()); //!< "YYYY-MM-DD HH:MM:SS" 形式で返却
}

std::wstring LocalTimePoint::SerializeW() const {
	return std::format(L"{} {}", date.SerializeW(), time.SerializeW()); //!< "YYYY-MM-DD HH:MM:SS" 形式で返却
}

LocalTimePoint LocalTimePoint::Deserialize(const std::string& s) {
	LocalTimePoint point = {};

	//!< 文字列の形式は "YYYY-MM-DD HH:MM:SS"
	point.date = Date::Deserialize(s.substr(0, 10)); //!< "YYYY-MM-DD"
	point.time = Time::Deserialize(s.substr(11, 8)); //!< "HH:MM:SS"

	return point;
}

LocalTimePoint LocalTimePoint::Now(const std::chrono::time_zone* zone) {

	std::chrono::zoned_time local{
		zone, std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now())
	};

	return LocalTimePoint{ local };
}

LocalTimePoint LocalTimePoint::Convert(
	const std::chrono::system_clock::time_point& time, const std::chrono::time_zone* zone) {

	std::chrono::zoned_time local{
		zone,  std::chrono::floor<std::chrono::seconds>(time)
	};

	return LocalTimePoint{ local };
}
