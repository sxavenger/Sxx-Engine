#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../AssetUtil.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/GraphicsUtil.h>

//* lib
#include <Lib/Format/Json/JsonNode.h>

//* DirectXTex
#include <DirectXTex/DirectXTex.h>

//* c++
#include <filesystem>
#include <variant>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// TextureMetadata structure
////////////////////////////////////////////////////////////////////////////////////////////
struct TextureMetadata final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ReferenceData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ReferenceData {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* reference option *//

		DirectX::TEX_FILTER_FLAGS GetFilterFlags() const;

		DirectX::WIC_FLAGS GetWICFlags() const;

		//* serialization option *//

		static json::node Serialize(const ReferenceData& data);

		static ReferenceData Deserialize(const json::node& node);

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::filesystem::path filepath; //!< 参照されているTextureのファイルパス

		Graphics::ColorEncoding encoding = Graphics::ColorEncoding::Lightness; //!< カラースペース
		bool isGenerateMipmaps           = true;                               //!< Mipmapを生成するかどうか

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* metadata option *//

	MetadataType GetType() const;

	const ReferenceData& GetReferenceData() const;

	//* serialization option *//

	static json::node Serialize(const TextureMetadata& metadata);

	static TextureMetadata Deserialize(const json::node& node);

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::variant<std::monostate, ReferenceData> data;

	//!< note: Textureは他ファイルの参照のみ.
	
private:
};

SXAVENGER_ENGINE_NAMESPACE_END


