#include "TomlNode.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TomlNode class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TomlNode::Merge(toml::table& dst, const toml::table& src) {
	for (const auto& [key, value] : src) {
		dst.insert_or_assign(key, value);
	}
}

void TomlNode::Merge(toml::table& dst, toml::table&& src) {
	for (auto& [key, value] : src) {
		dst.insert_or_assign(key, std::move(value));
	}
}

bool TomlNode::Contains(const toml::table& table, const std::string_view& path) {
	const auto node = table.at_path(path);
	return node.node() != nullptr;
}

RefPtr<const toml::node> TomlNode::GetNode(const toml::table& table, const std::string_view& path) {
	const auto node = table.at_path(path);
	return node.node();
}

const toml::table& TomlNode::GetTable(const toml::node& node) {
	STREAM_ASSERT(node.is_table(), "toml node is not table.");
	return *node.as_table();
}

const toml::array& TomlNode::GetArray(const toml::node& node) {
	STREAM_ASSERT(node.is_array(), "toml node is not array.");
	return *node.as_array();
}

int64_t TomlNode::GetIntegral(const toml::node& node) {
	STREAM_ASSERT(node.is<int64_t>(), "toml node is not integer.");
	return *node.value_exact<int64_t>();
}

std::optional<int64_t> TomlNode::FindIntegral(const toml::node& node) {
	if (!node.is<int64_t>()) {
		return std::nullopt;
	}

	return *node.value_exact<int64_t>();
}

double TomlNode::GetFloatingPoint(const toml::node& node) {
	STREAM_ASSERT(node.is<double>(), "toml node is not floating-point.");
	return *node.value_exact<double>();
}

std::optional<double> TomlNode::FindFloatingPoint(const toml::node& node) {
	if (!node.is<double>()) {
		return std::nullopt;
	}

	return *node.value_exact<double>();
}

bool TomlNode::GetBoolean(const toml::node& node) {
	STREAM_ASSERT(node.is<bool>(), "toml node is not boolean.");
	return *node.value_exact<bool>();
}

std::optional<bool> TomlNode::FindBoolean(const toml::node& node) {
	if (!node.is<bool>()) {
		return std::nullopt;
	}

	return *node.value_exact<bool>();
}


std::string_view TomlNode::GetString(const toml::node& node) {
	STREAM_ASSERT(node.is<std::string>(), "toml node is not string.");
	return *node.value_exact<std::string_view>();
}

std::optional<std::string_view> TomlNode::FindString(const toml::node& node) {
	if (!node.is<std::string>()) {
		return std::nullopt;
	}

	return *node.value_exact<std::string_view>();
}
