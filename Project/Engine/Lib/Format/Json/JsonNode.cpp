#include "JsonNode.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// JsonNode class methods
////////////////////////////////////////////////////////////////////////////////////////////

json::pointer JsonNode::CreatePointer(const std::string_view& path) {

	if (path.empty()) {
		return json::pointer();
	}

	std::string string(path);

	if (string[0] != '/') {
		string.insert(string.begin(), '/'); //!< json::pointerは先頭が'/'で始まる必要があるため、先頭に'/'を追加する.
	}

	return json::pointer(string);
}

void JsonNode::Merge(json::node& dst, const json::node& src) {
	dst.merge_patch(src);
}

bool JsonNode::Contains(const json::node& node, const std::string_view& path) {
	return node.contains(JsonNode::CreatePointer(path));
}

const json::node& JsonNode::GetNode(const json::node& node, const std::string_view& path) {
	json::pointer pointer = JsonNode::CreatePointer(path);
	STREAM_ASSERT(node.contains(pointer), "json path does not exist. path: {}", path);
	return node[pointer];
}
