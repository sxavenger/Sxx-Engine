#include "TypeInfo.h"

////////////////////////////////////////////////////////////////////////////////////////////
// TypeInfo structure methods
////////////////////////////////////////////////////////////////////////////////////////////

size_t TypeInfo::GetHashCode() const noexcept {
	if (info_ == nullptr) {
		return 0;
	}

	return info_->hash_code();
}

std::string_view TypeInfo::GetName() const noexcept {
	if (info_ == nullptr) {
		return std::string_view();
	}

	return info_->name();
}
