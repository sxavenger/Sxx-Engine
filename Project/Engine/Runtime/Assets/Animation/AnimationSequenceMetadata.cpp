#include "AnimationSequenceMetadata.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonReader.h>
#include <Lib/Format/Json/JsonWriter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [AnimationSequenceMetadata] ReferenceData structure methods
////////////////////////////////////////////////////////////////////////////////////////////

json::node AnimationSequenceMetadata::ReferenceData::Serialize(const ReferenceData& data) {
	json::node node = json::node::object();
	node["filepath"] = JsonWriter<std::filesystem::path>::Create(data.filepath);
	node["index"]    = JsonWriter<uint32_t>::Create(data.index);

	return node;
}

AnimationSequenceMetadata::ReferenceData AnimationSequenceMetadata::ReferenceData::Deserialize(const json::node& node) {
	ReferenceData data = {};
	data.filepath = JsonReader<std::filesystem::path>::Get(node, "filepath");
	data.index    = JsonReader<uint32_t>::Get(node, "index");

	return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationSequenceMetadata structure methods
////////////////////////////////////////////////////////////////////////////////////////////

MetadataType AnimationSequenceMetadata::GetType() const {
	return static_cast<MetadataType>(data.index());
}

const AnimationSequenceMetadata::ReferenceData& AnimationSequenceMetadata::GetReferenceData() const {
	STREAM_ASSERT(GetType() == MetadataType::Reference, "animation sequence metadata is not reference type.");
	return std::get<ReferenceData>(data);
}

json::node AnimationSequenceMetadata::Serialize(const AnimationSequenceMetadata& metadata) {

	json::node node = json::node::object();

	switch (metadata.GetType()) {
		case MetadataType::Reference:
			node["reference"] = ReferenceData::Serialize(std::get<ReferenceData>(metadata.data));
			break;

		case MetadataType::Inline:
			STREAM_LOG_ERROR("Assets::AnimationSequenceMetadata | animation sequence metadata type is inline. serialization is not supported.");
			node["inline"] = nullptr; //!< TODO: Inline情報の定義.
			break;

		default:
			STREAM_LOG_ERROR("Assets::AnimationSequenceMetadata | animation sequence metadata type is unknown.");
			node = nullptr;
			break;
	}

	return node;
}

AnimationSequenceMetadata AnimationSequenceMetadata::Deserialize(const json::node& node) {

	AnimationSequenceMetadata metadata = {};

	if (JsonNode::Contains(node, "reference")) {
		metadata.data = ReferenceData::Deserialize(JsonNode::GetNode(node, "reference"));
		return metadata;
	}

	if (JsonNode::Contains(node, "inline")) {
		metadata.data = InlineData(); //!< TODO: Inline情報の定義.
		STREAM_LOG_ERROR("Assets::AnimationSequenceMetadata | animation sequence metadata type is inline. deserialization is not supported.");
		return metadata;
	}

	STREAM_LOG_ERROR("Assets::AnimationSequenceMetadata | animation sequence metadata type is unknown.");
	return metadata;
}
