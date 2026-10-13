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
// SurfaceMaterialMetadata structure
////////////////////////////////////////////////////////////////////////////////////////////
struct SurfaceMaterialMetadata final {
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

		std::filesystem::path filepath; //!< 参照されているMaterialのファイルパス
		uint32_t index;                 //!< Materialのインデックス番号

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

	static json::node Serialize(const SurfaceMaterialMetadata& metadata);

	static SurfaceMaterialMetadata Deserialize(const json::node& node);

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::variant<std::monostate, ReferenceData, InlineData> data;

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
