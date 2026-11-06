#include "BoneMetadata.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonReader.h>
#include <Lib/Format/Json/JsonWriter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [BoneMetadata] ReferenceData structure methods
////////////////////////////////////////////////////////////////////////////////////////////

json::node BoneMetadata::ReferenceData::Serialize(const ReferenceData& data) {
	json::node node = json::node::object();
	node["filepath"] = JsonWriter<std::filesystem::path>::Create(data.filepath);

	return node;
}

BoneMetadata::ReferenceData BoneMetadata::ReferenceData::Deserialize(const json::node& node) {
	ReferenceData data = {};
	data.filepath = JsonReader<std::filesystem::path>::Get(node, "filepath");

	return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
// BoneMetadata structure methods
////////////////////////////////////////////////////////////////////////////////////////////

MetadataType BoneMetadata::GetType() const {
	return static_cast<MetadataType>(data.index());
}

const BoneMetadata::ReferenceData& BoneMetadata::GetReferenceData() const {
	STREAM_ASSERT(GetType() == MetadataType::Reference, "bone metadata is not reference type.");
	return std::get<ReferenceData>(data);
}

json::node BoneMetadata::Serialize(const BoneMetadata& metadata) {

	json::node node = json::node::object();

	switch (metadata.GetType()) {
		case MetadataType::Reference:
			node["reference"] = ReferenceData::Serialize(std::get<ReferenceData>(metadata.data));
			break;

		case MetadataType::Inline:
			STREAM_LOG_ERROR("Assets::BoneMetadata | bone metadata type is inline. serialization is not supported.");
			node["inline"] = nullptr; //!< TODO: Inline情報の定義.
			break;

		default:
			STREAM_LOG_ERROR("Assets::BoneMetadata | bone metadata type is unknown.");
			node = nullptr;
			break;
	}

	return node;
}

BoneMetadata BoneMetadata::Deserialize(const json::node& node) {

	BoneMetadata metadata = {};

	if (JsonNode::Contains(node, "reference")) {
		metadata.data = ReferenceData::Deserialize(JsonNode::GetNode(node, "reference"));
		return metadata;
	}

	if (JsonNode::Contains(node, "inline")) {
		metadata.data = InlineData(); //!< TODO: Inline情報の定義.
		STREAM_LOG_ERROR("Assets::BoneMetadata | bone metadata type is inline. deserialization is not supported.");
		return metadata;
	}

	STREAM_LOG_ERROR("Assets::BoneMetadata | bone metadata type is unknown.");
	return metadata;
}
