#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>
#include <chrono>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// LocalTimePoint structure
////////////////////////////////////////////////////////////////////////////////////////////
struct LocalTimePoint {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Date structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Date {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Date() noexcept = default;

		//* operator [assignment] <std::chrono::year_month_day> *//

		Date(const std::chrono::year_month_day& ymd) noexcept;
		Date& operator=(const std::chrono::year_month_day& ymd) noexcept;

		//* operator [assignment / move] <Date> *//

		Date(const Date&) noexcept            = default;
		Date& operator=(const Date&) noexcept = default;

		Date(Date&&) noexcept            = default;
		Date& operator=(Date&&) noexcept = default;

		//* operator [comparison] <Date> *//

		bool operator==(const Date& rhs) const noexcept;
		bool operator!=(const Date& rhs) const noexcept;

		//* serialize / deserialize *//

		std::string SerializeA() const;

		std::wstring SerializeW() const;

		static Date Deserialize(const std::string& s);

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint16_t year;  //!< [0000-9999] 年
		uint8_t  month; //!< [01-12] 月
		uint8_t  day;   //!< [01-31] 日

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Time structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Time {

		//=========================================================================================
		// public methods
		//=========================================================================================

		Time() noexcept = default;

		//* operator [assignment] <std::chrono::hh_mm_ss> *//

		Time(const std::chrono::hh_mm_ss<std::chrono::seconds>& hms) noexcept;
		Time& operator=(const std::chrono::hh_mm_ss<std::chrono::seconds>& hms) noexcept;

		//* operator [assignment / move] <Time> *//

		Time(const Time&) noexcept            = default;
		Time& operator=(const Time&) noexcept = default;

		Time(Time&&) noexcept            = default;
		Time& operator=(Time&&) noexcept = default;

		//* operator [comparison] <Time> *//

		bool operator==(const Time& rhs) const noexcept;
		bool operator!=(const Time& rhs) const noexcept;

		//* serialize / deserialize *//

		std::string SerializeA() const;

		std::wstring SerializeW() const;

		static Time Deserialize(const std::string& s);

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint8_t hour;   //!< [00-23] 時
		uint8_t minute; //!< [00-59] 分
		uint8_t second; //!< [00-59] 秒

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	LocalTimePoint() noexcept = default;

	//* operator [assignment] <std::chrono::zoned_time> *//

	LocalTimePoint(const std::chrono::zoned_time<std::chrono::seconds>& zone) noexcept;
	LocalTimePoint& operator=(const std::chrono::zoned_time<std::chrono::seconds>& zone) noexcept;

	//* operator [assignment / move] <LocalTimePoint> *//

	LocalTimePoint(const LocalTimePoint&) noexcept            = default;
	LocalTimePoint& operator=(const LocalTimePoint&) noexcept = default;

	LocalTimePoint(LocalTimePoint&&) noexcept            = default;
	LocalTimePoint& operator=(LocalTimePoint&&) noexcept = default;

	//* operator [comparison] <LocalTimePoint> *//

	bool operator==(const LocalTimePoint& rhs) const noexcept;
	bool operator!=(const LocalTimePoint& rhs) const noexcept;

	//* serialize / deserialize *//

	std::string SerializeA() const;

	std::wstring SerializeW() const;

	static LocalTimePoint Deserialize(const std::string& s);

	//* time point methods *//

	static LocalTimePoint Now(
		const std::chrono::time_zone* zone = std::chrono::current_zone()
	);

	static LocalTimePoint Convert(
		const std::chrono::system_clock::time_point& time,
		const std::chrono::time_zone* zone = std::chrono::current_zone()
	);

	//=========================================================================================
	// public variables
	//=========================================================================================

	Date date; //!< 日付
	Time time; //!< 時刻

};
