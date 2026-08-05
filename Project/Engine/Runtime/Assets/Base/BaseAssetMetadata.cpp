#include "BaseAssetMetadata.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Format/Json/JsonReader.h>
#include <Lib/Format/Json/JsonWriter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAssetMetadata structure methods
////////////////////////////////////////////////////////////////////////////////////////////

json::node BaseAssetMetadata::Serialize(const BaseAssetMetadata& metadata) {
	json::node node = json::node::object();
	node["uuid"] = JsonWriter<std::string_view>::Create(metadata.uuid.Serialize());
	node["type"] = JsonWriter<AssetType>::Create(metadata.type);

	return node;
}

BaseAssetMetadata BaseAssetMetadata::Deserialize(const std::filesystem::path& filepath, const json::node& node) {
	BaseAssetMetadata metadata = {};
	metadata.uuid     = Uuid::Deserialize(JsonReader<std::string_view>::Get(node, "uuid"));
	metadata.type     = JsonReader<AssetType>::Get(node, "type");
	metadata.filepath = filepath;

	return metadata;
}
