#include "SurfaceMaterialMetadata.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonReader.h>
#include <Lib/Format/Json/JsonWriter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [SurfaceMaterialMetadata] ReferenceData structure methods
////////////////////////////////////////////////////////////////////////////////////////////

json::node SurfaceMaterialMetadata::ReferenceData::Serialize(const ReferenceData& data) {
	json::node node = json::node::object();
	node["filepath"] = JsonWriter<std::filesystem::path>::Create(data.filepath);
	node["index"]    = JsonWriter<uint32_t>::Create(data.index);

	return node;
}

SurfaceMaterialMetadata::ReferenceData SurfaceMaterialMetadata::ReferenceData::Deserialize(const json::node& node) {
	ReferenceData data = {};
	data.filepath = JsonReader<std::filesystem::path>::Get(node, "filepath");
	data.index    = JsonReader<uint32_t>::Get(node, "index");
	
	return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceMaterialMetadata structure methods
////////////////////////////////////////////////////////////////////////////////////////////

MetadataType SurfaceMaterialMetadata::GetType() const {
	return static_cast<MetadataType>(data.index());
}

const SurfaceMaterialMetadata::ReferenceData& SurfaceMaterialMetadata::GetReferenceData() const {
	STREAM_ASSERT(GetType() == MetadataType::Reference, "surface material metadata is not reference type.");
	return std::get<ReferenceData>(data);
}

json::node SurfaceMaterialMetadata::Serialize(const SurfaceMaterialMetadata& metadata) {

	json::node node = json::node::object();

	switch (metadata.GetType()) {
		case MetadataType::Reference:
			node["reference"] = ReferenceData::Serialize(std::get<ReferenceData>(metadata.data));
			break;

		case MetadataType::Inline:
			STREAM_LOG_ERROR("Assets::SurfaceMaterialMetadata | surface material metadata type is inline. serialization is not supported.");
			node["inline"] = nullptr; //!< TODO: Inline情報の定義.
			break;

		default:
			STREAM_LOG_ERROR("Assets::SurfaceMaterialMetadata | surface material metadata type is unknown.");
			node = nullptr;
			break;
	}

	return node;
}

SurfaceMaterialMetadata SurfaceMaterialMetadata::Deserialize(const json::node& node) {

	SurfaceMaterialMetadata metadata = {};

	if (JsonNode::Contains(node, "reference")) {
		metadata.data = ReferenceData::Deserialize(JsonNode::GetNode(node, "reference"));
		return metadata;
	}

	if (JsonNode::Contains(node, "inline")) {
		metadata.data = InlineData(); //!< TODO: Inline情報の定義.
		STREAM_LOG_ERROR("Assets::StaticMeshMetadata | static mesh metadata type is inline. deserialization is not supported.");
		return metadata;
	}

	STREAM_LOG_ERROR("Assets::StaticMeshMetadata | static mesh metadata type is unknown.");
	return metadata;
}
