#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* json
#include "JsonNode.h"

//* lib
#include <Lib/Traits/Concept.h>
#include <Lib/Logger/StreamLogger.h>
#include <Lib/String/UnicodeConverter.h>
#include <Lib/Reflection/EnumUtil.h>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// JsonReader class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class JsonReader final {
public:
	static_assert(false, "unsupported json type.");
};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonReader class <Concept::Integral>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::Integral T>
class JsonReader<T> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief Integral型の取得.
	static T Get(const json::node& node, const std::string_view& path) {
		StreamLogger::Assert(JsonNode::Contains(node, path), std::format("json path does not exist. path: {}", path));

		const auto& n = JsonNode::GetNode(node, path);
		return JsonNode::GetIntegral<T>(n);
	}

	//! @brief Integral型の取得.
	//! @param[out] value 取得したIntegral型の値を格納する変数
	//! @retval true  pathに対応するnodeが存在し、Integral型である場合
	//! @retval false pathに対応するnodeが存在しない、またはIntegral型でない場合
	static bool Find(const json::node& node, const std::string_view& path, T& value) {
		if (!JsonNode::Contains(node, path)) {
			return false;
		}

		const auto& n = JsonNode::GetNode(node, path);

		std::optional<T> v = JsonNode::FindIntegral<T>(n);
		if (!v.has_value()) {
			return false;
		}

		value = v.value();
		return true;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonReader class <Concept::FloatingPoint>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::FloatingPoint T>
class JsonReader<T> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief FloatingPoint型の取得.
	static T Get(const json::node& node, const std::string_view& path) {
		StreamLogger::Assert(JsonNode::Contains(node, path), std::format("json path does not exist. path: {}", path));

		const auto& n = JsonNode::GetNode(node, path);
		return JsonNode::GetFloatingPoint<T>(n);
	}

	//! @brief FloatingPoint型の取得.
	//! @param[out] value 取得したFloatingPoint型の値を格納する変数
	//! @retval true  pathに対応するnodeが存在し、FloatingPoint型である場合
	//! @retval false pathに対応するnodeが存在しない、またはIntegral型でない場合
	static bool Find(const json::node& node, const std::string_view& path, T& value) {
		if (!JsonNode::Contains(node, path)) {
			return false;
		}

		const auto& n = JsonNode::GetNode(node, path);

		std::optional<T> v = JsonNode::FindFloatingPoint<T>(n);
		if (!v.has_value()) {
			return false;
		}

		value = v.value();
		return true;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonReader class <Concept::Boolean>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::Boolean T>
class JsonReader<T> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief FloatingPoint型の取得.
	static T Get(const json::node& node, const std::string_view& path) {
		StreamLogger::Assert(JsonNode::Contains(node, path), std::format("json path does not exist. path: {}", path));

		const auto& n = JsonNode::GetNode(node, path);
		return JsonNode::GetBoolean(n);
	}

	//! @brief FloatingPoint型の取得.
	//! @param[out] value 取得したFloatingPoint型の値を格納する変数
	//! @retval true  pathに対応するnodeが存在し、FloatingPoint型である場合
	//! @retval false pathに対応するnodeが存在しない、またはIntegral型でない場合
	static bool Find(const json::node& node, const std::string_view& path, T& value) {
		if (!JsonNode::Contains(node, path)) {
			return false;
		}

		const auto& n = JsonNode::GetNode(node, path);

		std::optional<T> v = JsonNode::GetBoolean(n);
		if (!v.has_value()) {
			return false;
		}

		value = v.value();
		return true;
	}

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonReader class <Concept::StringA>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::StringA T>
class JsonReader<T> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief StringA型の取得.
	static T Get(const json::node& node, const std::string_view& path) {
		StreamLogger::Assert(JsonNode::Contains(node, path), std::format("json path does not exist. path: {}", path));

		const auto& n = JsonNode::GetNode(node, path);
		return T(JsonNode::GetString(n));
	}

	//! @brief StringA型の取得.
	//! @param[out] value 取得したStringA型の値を格納する変数
	//! @retval true  pathに対応するnodeが存在し、StringA型である場合
	//! @retval false pathに対応するnodeが存在しない、またはIntegral型でない場合
	static bool Find(const json::node& node, const std::string_view& path, T& value) {
		if (!JsonNode::Contains(node, path)) {
			return false;
		}

		const auto& n = JsonNode::GetNode(node, path);

		std::optional<std::string_view> v = JsonNode::FindString(n);
		if (!v.has_value()) {
			return false;
		}

		value = v.value();
		return true;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonReader class <Concept::Enum>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::Enum T>
class JsonReader<T> final {
public:

	//! @brief Enum型の取得.
	static T Get(const json::node& node, const std::string_view& path) {
		StreamLogger::Assert(JsonNode::Contains(node, path), std::format("json path does not exist. path: {}", path));

		const auto& n = JsonNode::GetNode(node, path);
		std::string_view str = JsonNode::GetString(n);

		return EnumUtil<T>::GetEnum(str);
	}

	//! @brief StringA型の取得.
	//! @param[out] value 取得したStringA型の値を格納する変数
	//! @retval true  pathに対応するnodeが存在し、StringA型である場合
	//! @retval false pathに対応するnodeが存在しない、またはIntegral型でない場合
	static bool Find(const json::node& node, const std::string_view& path, T& value) {
		if (!JsonNode::Contains(node, path)) {
			return false;
		}

		const auto& n = JsonNode::GetNode(node, path);

		std::optional<std::string_view> v = JsonNode::FindString(n);
		if (!v.has_value()) {
			return false;
		}

		value = EnumUtil<T>::GetEnum(v.value());
		return true;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// JsonReader class <std::filesystem::path>
////////////////////////////////////////////////////////////////////////////////////////////
template <>
class JsonReader<std::filesystem::path> final {
public:

	//! @brief std::filesystem::pathの取得.
	static std::filesystem::path Get(const json::node& node, const std::string_view& path) {
		StreamLogger::Assert(JsonNode::Contains(node, path), std::format("json path does not exist. path: {}", path));

		const auto& n = JsonNode::GetNode(node, path);
		return JsonNode::GetString(n);
	}

	//! @brief std::filesystem::pathの取得.
	//! @param[out] value 取得したstd::filesystem::pathの値を格納する変数
	//! @retval true  pathに対応するnodeが存在し、std::filesystem::path型である場合
	//! @retval false pathに対応するnodeが存在しない、またはIntegral型でない場合
	static bool Find(const json::node& node, const std::string_view& path, std::filesystem::path& value) {
		if (!JsonNode::Contains(node, path)) {
			return false;
		}

		const auto& n = JsonNode::GetNode(node, path);

		std::optional<std::string_view> v = JsonNode::FindString(n);
		if (!v.has_value()) {
			return false;
		}

		value = v.value();
		return true;
	}

};
