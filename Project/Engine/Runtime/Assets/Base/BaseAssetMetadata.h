#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../AssetUtil.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Uuid/Uuid.h>
#include <Lib/Format/Json/JsonNode.h>

//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAssetMetadata structure
////////////////////////////////////////////////////////////////////////////////////////////
struct BaseAssetMetadata final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* serialization option *//

	static json::node Serialize(const BaseAssetMetadata& metadata);

	static BaseAssetMetadata Deserialize(const std::filesystem::path& filepath, const json::node& node);

	//=========================================================================================
	// public variables
	//=========================================================================================

	Uuid uuid;
	AssetType type;
	std::filesystem::path filepath;

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
