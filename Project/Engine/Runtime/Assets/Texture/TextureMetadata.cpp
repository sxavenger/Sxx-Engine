#include "TextureMetadata.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Format/Json/JsonReader.h>
#include <Lib/Format/Json/JsonWriter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [TextureMetadata] ReferenceData structure methods
////////////////////////////////////////////////////////////////////////////////////////////

DirectX::TEX_FILTER_FLAGS TextureMetadata::ReferenceData::GetFilterFlags() const {
	DirectX::TEX_FILTER_FLAGS flags = DirectX::TEX_FILTER_DEFAULT;

	if (encoding == Graphics::ColorEncoding::Lightness) {
		flags |= DirectX::TEX_FILTER_SRGB;
	}

	return flags;
}

DirectX::WIC_FLAGS TextureMetadata::ReferenceData::GetWICFlags() const {

	DirectX::WIC_FLAGS flags = DirectX::WIC_FLAGS_NONE;

	switch (encoding) {
		case Graphics::ColorEncoding::Intensity:
			flags |= DirectX::WIC_FLAGS_FORCE_RGB;
			break;

		case Graphics::ColorEncoding::Lightness:
			flags |= DirectX::WIC_FLAGS_FORCE_SRGB | DirectX::WIC_FLAGS_DEFAULT_SRGB;
			break;
	}

	return flags;
}

json::node TextureMetadata::ReferenceData::Serialize(const ReferenceData& data) {
	json::node node = json::node::object();
	node["filepath"]          = JsonWriter<std::filesystem::path>::Create(data.filepath);
	node["encoding"]          = JsonWriter<Graphics::ColorEncoding>::Create(data.encoding);
	node["isGenerateMipmaps"] = JsonWriter<bool>::Create(data.isGenerateMipmaps);

	return node;
}

TextureMetadata::ReferenceData TextureMetadata::ReferenceData::Deserialize(const json::node& node) {
	ReferenceData data = {};
	data.filepath          = JsonReader<std::filesystem::path>::Get(node, "filepath");
	data.encoding          = JsonReader<Graphics::ColorEncoding>::Get(node, "encoding");
	data.isGenerateMipmaps = JsonReader<bool>::Get(node, "isGenerateMipmaps");

	return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
// TextureMetadata structure methods
////////////////////////////////////////////////////////////////////////////////////////////

MetadataType TextureMetadata::GetType() const {
	return EnumUtil<MetadataType>::GetEnum(static_cast<EnumUtil<MetadataType>::Underlying>(data.index()));
}

const TextureMetadata::ReferenceData& TextureMetadata::GetReferenceData() const {
	STREAM_ASSERT(GetType() == MetadataType::Reference, "texture metadata is not reference type.");
	return std::get<ReferenceData>(data);
}

json::node TextureMetadata::Serialize(const TextureMetadata& metadata) {

	json::node node = json::node::object();

	switch (metadata.GetType()) {
		case MetadataType::Reference:
			node["reference"] = ReferenceData::Serialize(std::get<ReferenceData>(metadata.data));
			break;
			
		case MetadataType::Inline:
			STREAM_LOG_ERROR("Assets::TextureMetadata | texture metadata type is inline. serialization is not supported.");
			node["inline"] = nullptr;
			break;
			
		default:
			STREAM_LOG_ERROR("Assets::TextureMetadata | texture metadata type is unknown.");
			node = nullptr;
			break;
	}

	return node;
}

TextureMetadata TextureMetadata::Deserialize(const json::node& node) {

	TextureMetadata metadata = {};

	if (JsonNode::Contains(node, "reference")) { //!< reference型のmetadataを取得する.
		metadata.data = ReferenceData::Deserialize(JsonNode::GetNode(node, "reference"));
		return metadata;
	}

	STREAM_LOG_ERROR("Assets::TextureMetadata | texture metadata type is unknown.");
	return metadata;
}


