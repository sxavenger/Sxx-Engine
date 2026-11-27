#include "SnapshotDetailed.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SnapshotDetailed structure methods
////////////////////////////////////////////////////////////////////////////////////////////

SnapshotDetailed SnapshotDetailed::Current(const std::source_location& location, const std::stacktrace& stacktrace) {
	SnapshotDetailed snapshot;
	snapshot.snapshot = Snapshot::Current(location); //!< snapshotの取得.

	//!< stacktraceの取得.
	const std::filesystem::path current = std::filesystem::current_path();
	snapshot.stacktrace.reserve(stacktrace.size());

	for (const auto& frame : stacktrace) {
		StackFrame stackframe;
		stackframe.description = frame.description();
		stackframe.file        = std::filesystem::path(frame.source_file()).lexically_proximate(std::filesystem::current_path()); //!< current_path()からの相対パスに変換する.
		stackframe.line        = frame.source_line();

		snapshot.stacktrace.emplace_back(std::move(stackframe));
	}

	return snapshot;
}
