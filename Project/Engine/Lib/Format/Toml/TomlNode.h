#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* toml
#include <toml.hpp>

//* lib
#include <Lib/Pointer/ReferencePointer.h>

//* c++
#include <cstdint>
#include <string>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// TomlNode class
////////////////////////////////////////////////////////////////////////////////////////////
class TomlNode final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* table option *//

	//! @brief dstにsrcの内容をマージする.
	//! @note dstとsrcで同じキーが存在する場合、dstの値はsrcの値で上書きされる.
	static void Merge(toml::table& dst, const toml::table& src);

	//! @brief dstにsrcの内容をマージする.
	//! @note dstとsrcで同じキーが存在する場合、dstの値はsrcの値で上書きされる.
	static void Merge(toml::table& dst, toml::table&& src);

	//! @brief tableにpathが存在するかどうかを返す.
	static bool Contains(const toml::table& table, const std::string_view& path);

	//! @brief tableからpathに対応するnodeを取得する.
	//! @retval toml::node* pathに対応するnode
	//! @retval nullptr     pathに対応するnodeが存在しない場合.
	static RefPtr<const toml::node> GetNode(const toml::table& table, const std::string_view& path);

	//* node option *//

	//! @brief nodeからtableを取得する.
	static const toml::table& GetTable(const toml::node& node);

	//! @brief nodeからarrayを取得する.
	static const toml::array& GetArray(const toml::node& node);

	//! @brief nodeから整数値を取得する.
	static int64_t GetIntegral(const toml::node& node);

	//! @brief nodeから整数値を取得する.
	//! @retval int64_t nodeが整数値の場合
	//! @retval std::nullopt nodeが整数値でない場合
	static std::optional<int64_t> FindIntegral(const toml::node& node);

	//! @brief nodeから浮動小数点値を取得する.
	static double GetFloatingPoint(const toml::node& node);

	//! @brief nodeから浮動小数点値を取得する.
	//! @retval double nodeが浮動小数点値の場合
	//! @retval std::nullopt nodeが浮動小数点値でない場合
	static std::optional<double> FindFloatingPoint(const toml::node& node);

	//! @brief nodeからbooleanを取得する.
	static bool GetBoolean(const toml::node& node);

	//! @brief nodeからbooleanを取得する.
	//! @retval bool nodeがbooleanの場合
	//! @retval std::nullopt nodeがbooleanでない場合
	static std::optional<bool> FindBoolean(const toml::node& node);

	//! @brief nodeから文字列を取得する.
	static std::string_view GetString(const toml::node& node);

	//! @brief nodeから文字列を取得する.
	//! @retval std::string nodeが文字列の場合
	//! @retval std::nullopt nodeが文字列でない場合
	static std::optional<std::string_view> FindString(const toml::node& node);

private:
	// note: tomlでのpathの区切り文字は'.'である. (ex: "root.child1.child2")
};
