#include "BaseAsset.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAsset class methods
////////////////////////////////////////////////////////////////////////////////////////////

BaseAsset::BaseAsset(const BaseAssetMetadata& metadata) noexcept
	: type_(metadata.type), uuid_(metadata.uuid), filepath_(metadata.filepath) {
}

BaseAssetMetadata BaseAsset::GetMetadata() const {
	BaseAssetMetadata metadata = {};
	metadata.uuid     = uuid_;
	metadata.type     = type_;
	metadata.filepath = filepath_;

	return metadata;
}
