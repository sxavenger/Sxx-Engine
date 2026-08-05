#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* toml
#include "TomlNode.h"

//* lib
#include <Lib/Traits/Concept.h>
#include <Lib/Logger/StreamLogger.h>
#include <Lib/String/UnicodeConverter.h>
#include <Lib/Reflection/EnumUtil.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TomlReader class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class TomlReader final {
public:
	static_assert(false, "unsupported toml type.");
};

////////////////////////////////////////////////////////////////////////////////////////////
// TomlReader class <Concept::Integral>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::Integral T>
class TomlReader<T> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief Integral型の取得.
	static T Get(const toml::table& table, const std::string_view& path) {
		StreamLogger::Assert(TomlNode::Contains(table, path), std::format("toml path does not exist. path: {}", path));

		const auto node = TomlNode::GetNode(table, path);
		return static_cast<T>(TomlNode::GetIntegral(node.GetRef()));
	}

	//! @brief Integral型の取得.
	//! @param[out] value 取得したIntegral型の値を格納する変数
	//! @retval true  pathに対応するnodeが存在し、Integral型である場合
	//! @retval false pathに対応するnodeが存在しない、またはIntegral型でない場合
	static bool Find(const toml::table& table, const std::string_view& path, T& value) {
		if (!TomlNode::Contains(table, path)) {
			return false;
		}

		const auto node = TomlNode::GetNode(table, path);

		std::optional<int64_t> v = TomlNode::FindIntegral(node.GetRef());
		if (!v.has_value()) {
			return false;
		}

		value = static_cast<T>(v.value());
		return true;
	}

	//! @brief Integral型の配列を取得.
	static std::vector<T> GetVector(const toml::table& table, const std::string_view& path) {
		StreamLogger::Assert(TomlNode::Contains(table, path), std::format("toml path does not exist. path: {}", path));

		const auto node = TomlNode::GetNode(table, path);

		StreamLogger::Assert(node->is_array(), std::format("toml node is not an array. path: {}", path));
		const auto& array = TomlNode::GetArray(node.GetRef());

		std::vector<T> v(array.size());
		for (size_t i = 0; i < array.size(); i++) {
			v[i] = static_cast<T>(TomlNode::GetIntegral(array.at(i)));
		}

		return v;
	}

	//! @brief Integral型の配列を取得.
	//! @param[out] values 取得したIntegral型の配列を格納する変数
	//! @retval true  pathに対応するnodeが存在し、Integral型の配列である場合
	//! @retval false pathに対応するnodeが存在しない、またはIntegral型の配列でない場合
	static bool FindVector(const toml::table& table, const std::string_view& path, std::vector<T>& values) {
		if (!TomlNode::Contains(table, path)) {
			return false;
		}

		const auto node = TomlNode::GetNode(table, path);
		if (!node->is_array()) {
			return false;
		}

		const auto& array = TomlNode::GetArray(node.GetRef());

		values = std::vector<T>(array.size());
		for (size_t i = 0; i < array.size(); i++) {
			int64_t v = TomlNode::GetIntegral(array.at(i));
			values[i]  = static_cast<T>(v);
		}

		return true;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// TomlReader structure <Concept::FloatingPoint>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::FloatingPoint T>
struct TomlReader<T> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief FloatingPoint型の取得.
	static T Get(const toml::table& table, const std::string_view& path) {
		StreamLogger::Assert(TomlNode::Contains(table, path), std::format("toml path does not exist. path: {}", path));

		const auto node = TomlNode::GetNode(table, path);
		return static_cast<T>(TomlNode::GetFloatingPoint(node.GetRef()));
	}

	//! @brief FloatingPoint型の取得.
	//! @param[out] value 取得したFloatingPoint型の値を格納する変数
	//! @retval true  pathに対応するnodeが存在し、FloatingPoint型である場合
	//! @retval false pathに対応するnodeが存在しない、またはFloatingPoint型でない場合
	static bool Find(const toml::table& table, const std::string_view& path, T& value) {
		if (!TomlNode::Contains(table, path)) {
			return false;
		}

		const auto node = TomlNode::GetNode(table, path);

		std::optional<double> v = TomlNode::FindFloatingPoint(node.GetRef());
		if (!v.has_value()) {
			return false;
		}

		value = static_cast<T>(v.value());
		return true;
	}

	//! @brief FloatingPoint型の配列を取得.
	static std::vector<T> GetVector(const toml::table& table, const std::string_view& path) {
		StreamLogger::Assert(TomlNode::Contains(table, path), std::format("toml path does not exist. path: {}", path));

		const auto node = TomlNode::GetNode(table, path);

		StreamLogger::Assert(node->is_array(), std::format("toml node is not an array. path: {}", path));
		const auto& array = TomlNode::GetArray(node.GetRef());

		std::vector<T> v(array.size());
		for (size_t i = 0; i < array.size(); i++) {
			v[i] = static_cast<T>(TomlNode::GetFloatingPoint(array.at(i)));
		}

		return v;
	}

	//! @brief FloatingPoint型の配列を取得.
	//! @param[out] value 取得したFloatingPoint型の配列を格納する変数
	//! @retval true  pathに対応するnodeが存在し、FloatingPoint型の配列である場合
	//! @retval false pathに対応するnodeが存在しない、またはFloatingPoint型の配列でない場合
	static bool FindVector(const toml::table& table, const std::string_view& path, std::vector<T>& value) {
		if (!TomlNode::Contains(table, path)) {
			return false;
		}

		const auto node = TomlNode::GetNode(table, path);
		if (!node->is_array()) {
			return false;
		}

		const auto& array = TomlNode::GetArray(node.GetRef());

		value = std::vector<T>(array.size());
		for (size_t i = 0; i < array.size(); i++) {
			double v = TomlNode::GetFloatingPoint(array.at(i));
			value[i]  = static_cast<T>(v);
		}

		return true;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// TomlReader structure <Concept::Boolean>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::Boolean T>
struct TomlReader<T> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief Boolean型の取得.
	static T Get(const toml::table& table, const std::string_view& path) {
		StreamLogger::Assert(TomlNode::Contains(table, path), std::format("toml path does not exist. path: {}", path));

		const auto node = TomlNode::GetNode(table, path);
		return static_cast<T>(TomlNode::GetBoolean(node.GetRef()));
	}

	//! @brief Boolean型の取得.
	//! @param[out] value 取得したBoolean型の値を格納する変数
	//! @retval true  pathに対応するnodeが存在し、Boolean型である場合
	//! @retval false pathに対応するnodeが存在しない、またはBoolean型でない場合
	static bool Find(const toml::table& table, const std::string_view& path, T& value) {
		if (!TomlNode::Contains(table, path)) {
			return false;
		}

		const auto node = TomlNode::GetNode(table, path);

		std::optional<bool> v = TomlNode::FindBoolean(node.GetRef());
		if (!v.has_value()) {
			return false;
		}

		value = static_cast<T>(v.value());
		return true;
	}

	//! @brief Boolean型の配列を取得.
	static std::vector<T> GetVector(const toml::table& table, const std::string_view& path) {
		StreamLogger::Assert(TomlNode::Contains(table, path), std::format("toml path does not exist. path: {}", path));

		const auto node = TomlNode::GetNode(table, path);

		StreamLogger::Assert(node->is_array(), std::format("toml node is not an array. path: {}", path));
		const auto& array = TomlNode::GetArray(node.GetRef());

		std::vector<T> v(array.size());
		for (size_t i = 0; i < array.size(); i++) {
			v[i] = static_cast<T>(TomlNode::GetBoolean(array.at(i)));
		}

		return v;
	}

	//! @brief Boolean型の配列を取得.
	//! @param[out] value 取得したBoolean型の配列を格納する変数
	//! @retval true  pathに対応するnodeが存在し、Boolean型の配列である場合
	//! @retval false pathに対応するnodeが存在しない、またはBoolean型の配列でない場合
	static bool FindVector(const toml::table& table, const std::string_view& path, std::vector<T>& value) {
		if (!TomlNode::Contains(table, path)) {
			return false;
		}

		const auto node = TomlNode::GetNode(table, path);
		if (!node->is_array()) {
			return false;
		}

		const auto& array = TomlNode::GetArray(node.GetRef());

		value = std::vector<T>(array.size());
		for (size_t i = 0; i < array.size(); i++) {
			bool v = TomlNode::GetBoolean(array.at(i));
			value[i] = static_cast<T>(v);
		}

		return true;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// TomlReader structure <Concept::StringA>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::StringA T>
struct TomlReader<T> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief StringA型の取得.
	static T Get(const toml::table& table, const std::string_view& path) {
		StreamLogger::Assert(TomlNode::Contains(table, path), std::format("toml path does not exist. path: {}", path));

		const auto node = TomlNode::GetNode(table, path);
		return T(TomlNode::GetString(node.GetRef()));
	}

	//! @brief StringA型の取得.
	//! @param[out] value 取得したStringA型の値を格納する変数
	//! @retval true  pathに対応するnodeが存在し、StringA型である場合
	//! @retval false pathに対応するnodeが存在しない、またはStringA型でない場合
	static bool Find(const toml::table& table, const std::string_view& path, T& value) {
		if (!TomlNode::Contains(table, path)) {
			return false;
		}

		const auto node = TomlNode::GetNode(table, path);

		std::optional<std::string_view> v = TomlNode::FindString(node.GetRef());
		if (!v.has_value()) {
			return false;
		}

		value = v.value();
		return true;
	}

	//! @brief StringA型の配列を取得.
	static std::vector<T> GetVector(const toml::table& table, const std::string_view& path) {
		StreamLogger::Assert(TomlNode::Contains(table, path), std::format("toml path does not exist. path: {}", path));

		const auto node = TomlNode::GetNode(table, path);

		StreamLogger::Assert(node->is_array(), std::format("toml node is not an array. path: {}", path));
		const auto& array = TomlNode::GetArray(node.GetRef());

		std::vector<T> v(array.size());
		for (size_t i = 0; i < array.size(); i++) {
			v[i] = TomlNode::GetString(array.at(i));
		}

		return v;
	}

	//! @brief StringA型の配列を取得.
	//! @param[out] value 取得したStringA型の配列を格納する変数
	//! @retval true  pathに対応するnodeが存在し、StringA型の配列である場合
	//! @retval false pathに対応するnodeが存在しない、またはStringA型の配列でない場合
	static bool FindVector(const toml::table& table, const std::string_view& path, std::vector<T>& value) {
		if (!TomlNode::Contains(table, path)) {
			return false;
		}

		const auto node = TomlNode::GetNode(table, path);
		if (!node->is_array()) {
			return false;
		}

		const auto& array = TomlNode::GetArray(node.GetRef());

		value = std::vector<T>(array.size());
		for (size_t i = 0; i < array.size(); i++) {
			std::string v = TomlNode::GetString(array.at(i));
			value[i] = v;
		}

		return true;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// TomlReader structure <Concept::StringW>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::StringW T>
struct TomlReader<T> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief StringW型の取得.
	static T Get(const toml::table& table, const std::string_view& path) {
		StreamLogger::Assert(TomlNode::Contains(table, path), std::format("toml path does not exist. path: {}", path));

		const auto node = TomlNode::GetNode(table, path);
		return UnicodeConverter::ConvertW(TomlNode::GetString(node.GetRef()));
	}

	//! @brief StringW型の取得.
	//! @param[out] value 取得したStringW型の値を格納する変数
	//! @retval true  pathに対応するnodeが存在し、StringW型である場合
	//! @retval false pathに対応するnodeが存在しない、またはStringW型でない場合
	static bool Find(const toml::table& table, const std::string_view& path, T& value) {
		if (!TomlNode::Contains(table, path)) {
			return false;
		}

		const auto node = TomlNode::GetNode(table, path);

		std::optional<std::string> v = TomlNode::FindString(node.GetRef());
		if (!v.has_value()) {
			return false;
		}

		value = UnicodeConverter::ConvertW(v.value());
		return true;
	}

	//! @brief StringW型の配列を取得.
	static std::vector<T> GetVector(const toml::table& table, const std::string_view& path) {
		StreamLogger::Assert(TomlNode::Contains(table, path), std::format("toml path does not exist. path: {}", path));

		const auto node = TomlNode::GetNode(table, path);

		StreamLogger::Assert(node->is_array(), std::format("toml node is not an array. path: {}", path));
		const auto& array = TomlNode::GetArray(node.GetRef());

		std::vector<T> v(array.size());
		for (size_t i = 0; i < array.size(); i++) {
			v[i] = UnicodeConverter::ConvertW(TomlNode::GetString(array.at(i)));
		}

		return v;
	}

	//! @brief StringW型の配列を取得.
	//! @param[out] value 取得したStringW型の配列を格納する変数
	//! @retval true  pathに対応するnodeが存在し、StringW型の配列である場合
	//! @retval false pathに対応するnodeが存在しない、またはStringW型の配列でない場合
	static bool FindVector(const toml::table& table, const std::string_view& path, std::vector<T>& value) {
		if (!TomlNode::Contains(table, path)) {
			return false;
		}

		const auto node = TomlNode::GetNode(table, path);
		if (!node->is_array()) {
			return false;
		}

		const auto& array = TomlNode::GetArray(node.GetRef());

		value = std::vector<T>(array.size());
		for (size_t i = 0; i < array.size(); i++) {
			std::string v = TomlNode::GetString(array.at(i));
			value[i] = UnicodeConverter::ConvertW(v);
		}

		return true;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// TomlReader structure <Concept::Enum>
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::Enum T>
struct TomlReader<T> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief Enum型の取得.
	static T Get(const toml::table& table, const std::string_view& path) {
		StreamLogger::Assert(TomlNode::Contains(table, path), std::format("toml path does not exist. path: {}", path));

		const auto node  = TomlNode::GetNode(table, path);
		std::string name = TomlNode::GetString(node.GetRef());

		StreamLogger::Assert(EnumUtil<T>::Contains(name), std::format("toml enum name is invalid. name: {}", name));
		return EnumUtil<T>::GetEnum(name);
	}

	//! @brief Enum型の取得.
	//! @param[out] value 取得したEnum型の値を格納する変数
	//! @retval true  pathに対応するnodeが存在し、Enum型である場合
	//! @retval false pathに対応するnodeが存在しない、またはEnum型でない場合
	static bool Find(const toml::table& table, const std::string_view& path, T& value) {
		if (!TomlNode::Contains(table, path)) {
			return false;
		}

		const auto node = TomlNode::GetNode(table, path);

		std::optional<std::string_view> name = TomlNode::FindString(node.GetRef());
		if (!name.has_value()) {
			return false;
		}

		StreamLogger::Assert(EnumUtil<T>::Contains(name.value()), std::format("toml enum name is invalid. name: {}", name.value()));
		value = EnumUtil<T>::GetEnum(name.value());

		return true;
	}

	//! @brief Enum型の配列を取得.
	static std::vector<T> GetVector(const toml::table& table, const std::string_view& path) {
		StreamLogger::Assert(TomlNode::Contains(table, path), std::format("toml path does not exist. path: {}", path));

		const auto node = TomlNode::GetNode(table, path);

		StreamLogger::Assert(node->is_array(), std::format("toml node is not an array. path: {}", path));
		const auto& array = TomlNode::GetArray(node.GetRef());

		std::vector<T> v(array.size());
		for (size_t i = 0; i < array.size(); i++) {
			std::string_view name = TomlNode::GetString(array.at(i));
			v[i] = EnumUtil<T>::GetEnum(name);
		}

		return v;
	}

	//! @brief Enum型の配列を取得.
	//! @param[out] value 取得したEnum型の配列を格納する変数
	//! @retval true  pathに対応するnodeが存在し、Enum型の配列である場合
	//! @retval false pathに対応するnodeが存在しない、またはEnum型の配列でない場合
	static bool FindVector(const toml::table& table, const std::string_view& path, std::vector<T>& value) {
		if (!TomlNode::Contains(table, path)) {
			return false;
		}

		const auto node = TomlNode::GetNode(table, path);
		if (!node->is_array()) {
			return false;
		}

		const auto& array = TomlNode::GetArray(node.GetRef());

		value = std::vector<T>(array.size());
		for (size_t i = 0; i < array.size(); i++) {
			std::string_view name = TomlNode::GetString(array.at(i));

			StreamLogger::Assert(EnumUtil<T>::Contains(name), std::format("toml enum name is invalid. name: {}", name));
			value[i] = EnumUtil<T>::GetEnum(name);
		}

		return true;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// TomlReader structure <std::filesystem::path>
////////////////////////////////////////////////////////////////////////////////////////////
template <>
struct TomlReader<std::filesystem::path> {
public:

	//! @brief std::filesystem::pathの取得.
	static std::filesystem::path Get(const toml::table& table, const std::string_view& path) {
		StreamLogger::Assert(TomlNode::Contains(table, path), std::format("toml path does not exist. path: {}", path));

		const auto node = TomlNode::GetNode(table, path);
		return TomlNode::GetString(node.GetRef());
	}

	//! @brief std::filesystem::pathの取得.
	//! @param[out] value 取得したstd::filesystem::pathの値を格納する変数
	//! @retval true  pathに対応するnodeが存在し、std::filesystem::path型である場合
	//! @retval false pathに対応するnodeが存在しない、またはstd::filesystem::path型でない場合
	static bool Find(const toml::table& table, const std::string_view& path, std::filesystem::path& value) {
		if (!TomlNode::Contains(table, path)) {
			return false;
		}

		const auto node = TomlNode::GetNode(table, path);

		std::optional<std::string_view> v = TomlNode::FindString(node.GetRef());
		if (!v.has_value()) {
			return false;
		}

		value = v.value();
		return true;
	}

};
