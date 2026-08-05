#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* json
#include "JsonUtil.h"

//* lib
#include <Lib/Traits/Concept.h>
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// JsonNode class
////////////////////////////////////////////////////////////////////////////////////////////
class JsonNode final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* pointer option *//

	//! @brief json::pointerを作成する.
	static json::pointer CreatePointer(const std::string_view& path);

	//* node option *//

	//! @brief dstにsrcの内容をマージする.
	//! @note dstとsrcで同じキーが存在する場合、dstの値はsrcの値で上書きされる.
	static void Merge(json::node& dst, const json::node& src);

	//! @brief nodeにpathが存在するかどうかを返す.
	static bool Contains(const json::node& node, const std::string_view& path);

	//! @brief nodeからpathに対応するnodeを取得する.
	static const json::node& GetNode(const json::node& node, const std::string_view& path);

	//! @brief nodeから整数値を取得する.
	template <Concept::Integral T>
	static T GetIntegral(const json::node& node);

	//! @brief nodeから整数値を取得する.
	//! @retval T nodeが整数値の場合
	//! @retval std::nullopt nodeが整数値でない場合
	template <Concept::Integral T>
	static std::optional<T> FindIntegral(const json::node& node);

	//! @brief 整数値からnodeを作成する.
	template <Concept::Integral T>
	static json::node CreateIntegral(T value);

	//! @brief nodeから浮動小数点値を取得する.
	template <Concept::FloatingPoint T>
	static T GetFloatingPoint(const json::node& node);

	//! @brief nodeから浮動小数点値を取得する.
	//! @retval T nodeが浮動小数点値の場合
	//! @retval std::nullopt nodeが浮動小数点値でない場合
	template <Concept::FloatingPoint T>
	static std::optional<T> FindFloatingPoint(const json::node& node);

	//! @brief 浮動小数点値からnodeを作成する.
	template <Concept::FloatingPoint T>
	static json::node CreateFloatingPoint(T value);

	//! @brief nodeからBoolean型の値を取得する.
	static bool GetBoolean(const json::node& node);

	//! @brief nodeからBoolean型の値を取得する.
	//! @retval bool nodeがBoolean型の場合
	//! @retval std::nullopt nodeがBoolean型でない場合
	static std::optional<bool> FindBoolean(const json::node& node);

	//! @brief Boolean型の値からnodeを作成する.
	static json::node CreateBoolean(bool value);

	//! @brief nodeからString型の値を取得する.
	static std::string_view GetString(const json::node& node);

	//! @brief nodeからString型の値を取得する.
	//! @retval std::string nodeがString型の場合
	//! @retval std::nullopt nodeがString型でない場合
	static std::optional<std::string_view> FindString(const json::node& node);

	//! @brief String型の値からnodeを作成する.
	static json::node CreateString(const std::string_view& value);

private:
	// note: jsonでのpathの区切り文字は'/'である. (ex: "/root/child1/child2")
	//!< JsonNode::CreatePointer(...)で作成されるjson::pointerは、先頭が'/'で始まる必要があるため、先頭に'/'を追加する.
};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonNode class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Concept::Integral T>
inline T JsonNode::GetIntegral(const json::node& node) {
	StreamLogger::Assert(node.is_number_integer(), "json node is not an integer");
	return node.get<T>();
}

template <Concept::Integral T>
inline std::optional<T> JsonNode::FindIntegral(const json::node& node) {
	if (!node.is_number_integer()) {
		return std::nullopt;
	}

	return std::optional<T>(node.get<T>());
}

template <Concept::Integral T>
inline json::node JsonNode::CreateIntegral(T value) {
	return json::node(value);
}

template <Concept::FloatingPoint T>
inline T JsonNode::GetFloatingPoint(const json::node& node) {
	StreamLogger::Assert(node.is_number_float(), "json node is not a floating-point");
	return node.get<T>();
}

template <Concept::FloatingPoint T>
inline std::optional<T> JsonNode::FindFloatingPoint(const json::node& node) {
	if (!node.is_number_float()) {
		return std::nullopt;
	}

	return std::optional<T>(node.get<T>());
}

template <Concept::FloatingPoint T>
inline json::node JsonNode::CreateFloatingPoint(T value) {
	return json::node(value);
}

inline bool JsonNode::GetBoolean(const json::node& node) {
	StreamLogger::Assert(node.is_boolean(), "json node is not a boolean");
	return node.get<bool>();
}

inline std::optional<bool> JsonNode::FindBoolean(const json::node& node) {
	if (!node.is_boolean()) {
		return std::nullopt;
	}

	return std::optional<bool>(node.get<bool>());
}

inline json::node JsonNode::CreateBoolean(bool value) {
	return json::node(value);
}

inline std::string_view JsonNode::GetString(const json::node& node) {
	StreamLogger::Assert(node.is_string(), "json node is not a string");
	return node.get_ref<const std::string&>();
}

inline std::optional<std::string_view> JsonNode::FindString(const json::node& node) {
	if (!node.is_string()) {
		return std::nullopt;
	}

	return node.get_ref<const std::string&>();
}

inline json::node JsonNode::CreateString(const std::string_view& value) {
	return json::node(value);
}
