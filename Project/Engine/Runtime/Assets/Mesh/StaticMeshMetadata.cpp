#include "StaticMeshMetadata.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonReader.h>
#include <Lib/Format/Json/JsonWriter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// StaticMeshMetadata structure methods
////////////////////////////////////////////////////////////////////////////////////////////

json::node StaticMeshMetadata::ReferenceData::Serialize(const ReferenceData& data) {
	json::node node = json::node::object();
	node["filepath"] = JsonWriter<std::filesystem::path>::Create(data.filepath);
	node["index"]    = JsonWriter<uint32_t>::Create(data.index);

	return node;
}

StaticMeshMetadata::ReferenceData StaticMeshMetadata::ReferenceData::Deserialize(const json::node& node) {
	ReferenceData data = {};
	data.filepath = JsonReader<std::filesystem::path>::Get(node, "filepath");
	data.index    = JsonReader<uint32_t>::Get(node, "index");
	
	return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
// StaticMeshMetadata structure methods
////////////////////////////////////////////////////////////////////////////////////////////

MetadataType StaticMeshMetadata::GetType() const {
	return static_cast<MetadataType>(data.index());
}

const StaticMeshMetadata::ReferenceData& StaticMeshMetadata::GetReferenceData() const {
	StreamLogger::Assert(GetType() == MetadataType::Reference, "static mesh metadata is not reference type.");
	return std::get<ReferenceData>(data);
}

json::node StaticMeshMetadata::Serialize(const StaticMeshMetadata& metadata) {

	json::node node = json::node::object();

	switch (metadata.GetType()) {
		case MetadataType::Reference:
			node["reference"] = ReferenceData::Serialize(std::get<ReferenceData>(metadata.data));
			break;

		case MetadataType::Inline:
			StreamLogger::Error("Assets::StaticMeshMetadata | static mesh metadata type is inline. serialization is not supported.");
			node["inline"] = nullptr; //!< TODO: Inline情報の定義.
			break;

		default:
			StreamLogger::Error("Assets::StaticMeshMetadata | static mesh metadata type is unknown.");
			node = nullptr;
			break;
	}

	return node;
}

StaticMeshMetadata StaticMeshMetadata::Deserialize(const json::node& node) {

	StaticMeshMetadata metadata = {};

	if (JsonNode::Contains(node, "reference")) {
		metadata.data = ReferenceData::Deserialize(JsonNode::GetNode(node, "reference"));
		return metadata;
	}

	if (JsonNode::Contains(node, "inline")) {
		metadata.data = InlineData(); //!< TODO: Inline情報の定義.
		StreamLogger::Error("Assets::StaticMeshMetadata | static mesh metadata type is inline. deserialization is not supported.");
		return metadata;
	}

	StreamLogger::Error("Assets::StaticMeshMetadata | static mesh metadata type is unknown.");
	return metadata;
}
