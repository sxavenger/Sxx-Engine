#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* snapshot
#include "Snapshot.h"

//* c++
#include <stacktrace>
#include <span>
#include <limits>

////////////////////////////////////////////////////////////////////////////////////////////
// SnapshotDetailed structure
////////////////////////////////////////////////////////////////////////////////////////////
struct SnapshotDetailed final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// StackFrame structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct StackFrame final {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		const std::string& GetDescription() const noexcept { return description; }

		const std::filesystem::path& GetSourceFile() const noexcept { return file; }

		uint_least32_t GetSourceLine() const noexcept { return line; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::string description;
		std::filesystem::path file;
		uint_least32_t line;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	SnapshotDetailed() noexcept  = default;
	~SnapshotDetailed() noexcept = default;

	//* snapshot methods *//

	std::thread::id GetThreadId() const noexcept { return snapshot.id; }

	const LocalTimePoint& GetTimestamp() const noexcept { return snapshot.timestamp; }

	const std::filesystem::path& GetSourceFile() const noexcept { return snapshot.file; }

	const std::string_view& GetSourceFunction() const noexcept { return snapshot.function; }

	uint_least32_t GetSourceLine() const noexcept { return snapshot.line; }

	size_t GetStackFrameCount() const noexcept { return stacktrace.size(); }

	const StackFrame& GetStackFrame(size_t index) const noexcept { return stacktrace[index]; }

	std::span<const StackFrame> GetStacktrace(size_t skip = 0, size_t count = std::numeric_limits<size_t>::max()) const noexcept { return std::span<const StackFrame>(stacktrace).subspan(skip, count); }

	//* operator [copy / move] <SnapshotDetailed> *//

	SnapshotDetailed(const SnapshotDetailed&) noexcept            = default;
	SnapshotDetailed& operator=(const SnapshotDetailed&) noexcept = default;

	SnapshotDetailed(SnapshotDetailed&&) noexcept            = default;
	SnapshotDetailed& operator=(SnapshotDetailed&&) noexcept = default;

	//* static methods *//

	static SnapshotDetailed Current(const std::source_location& location = std::source_location::current(), const std::stacktrace& stacktrace = std::stacktrace::current());

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* snapshot information *//

	Snapshot snapshot;

	//* stacktrace information *//

	std::vector<StackFrame> stacktrace;
	
};
