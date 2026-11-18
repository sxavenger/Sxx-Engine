#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Time/LocalTimePoint.h>

//* c++
#include <thread>
#include <source_location>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Snapshot structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Snapshot final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	Snapshot() noexcept  = default;
	~Snapshot() noexcept = default;

	//* snapshot methods *//

	std::thread::id GetThreadId() const noexcept { return id; }

	const LocalTimePoint& GetTimestamp() const noexcept { return timestamp; }

	const std::filesystem::path& GetSourceFile() const noexcept { return file; }

	const std::string_view& GetSourceFunction() const noexcept { return function; }

	uint_least32_t GetSourceLine() const noexcept { return line; }

	//* operator [copy / move] <Snapshot> *//

	Snapshot(const Snapshot&) noexcept            = default;
	Snapshot& operator=(const Snapshot&) noexcept = default;

	Snapshot(Snapshot&&) noexcept            = default;
	Snapshot& operator=(Snapshot&&) noexcept = default;

	//* static methods *//

	static Snapshot Current(const std::source_location& location = std::source_location::current());

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* runtime information *//

	std::thread::id id;
	LocalTimePoint timestamp;

	//* source location information *//

	std::filesystem::path file;
	std::string_view function;
	uint_least32_t line;

};
