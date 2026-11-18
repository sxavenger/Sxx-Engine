#include "Snapshot.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Snapshot structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Snapshot Snapshot::Current(const std::source_location& location) {
	Snapshot snapshot;
	snapshot.id        = std::this_thread::get_id();
	snapshot.timestamp = LocalTimePoint::Now();
	snapshot.file      = std::filesystem::path(location.file_name()).lexically_proximate(std::filesystem::current_path()); //!< current_path()からの相対パスに変換する.
	snapshot.function  = location.function_name();
	snapshot.line      = location.line();

	return snapshot;
}
