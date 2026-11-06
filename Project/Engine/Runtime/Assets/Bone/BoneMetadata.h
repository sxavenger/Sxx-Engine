#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../AssetUtil.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Format/Json/JsonNode.h>

//* c++
#include <filesystem>
#include <variant>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// BoneMetadata structure
////////////////////////////////////////////////////////////////////////////////////////////
struct BoneMetadata final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ReferenceData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ReferenceData {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* serialization option *//

		static json::node Serialize(const ReferenceData& data);

		static ReferenceData Deserialize(const json::node& node);

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::filesystem::path filepath; //!< 参照されているBornのファイルパス

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// InlineData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct InlineData {
	public:
		// TODO: Inline情報の定義.
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* metadata type *//

	MetadataType GetType() const;

	const ReferenceData& GetReferenceData() const;

	//* serialization option *//

	static json::node Serialize(const BoneMetadata& metadata);

	static BoneMetadata Deserialize(const json::node& node);

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::variant<std::monostate, ReferenceData, InlineData> data;

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
