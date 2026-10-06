#include "Configuration.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Toml/TomlFile.h>
#include <Lib/Format/Toml/TomlNode.h>

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// [Configuration] Path structure methods
////////////////////////////////////////////////////////////////////////////////////////////

const std::string Configuration::Path::GetPath() const {
	return std::string(path); //!< パスの取得.
}

const std::string Configuration::Path::GetProfilePath() const {
	return std::format("{}.{}", path, _PROFILE); //!< プロファイルパスの取得
}

////////////////////////////////////////////////////////////////////////////////////////////
// Configuration class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Configuration::Load(const std::filesystem::path& filepath) {

	toml::table data;
	bool isSuccess = TomlFile::TryLoad(filepath, data); //!< tomlの取得.

	if (!isSuccess) {
		STREAM_LOG_WARNING(
			"Configuration | failed to load config. filepath: {}", filepath.generic_string()
		);
		return; //!< 設定の読み込みに失敗.
	}

	//!< 現在の設定に上書きする.
	TomlNode::Merge(config_, std::move(data));

	STREAM_LOG_INFO(
		"Configuration | successfully loaded config. filepath: {}", filepath.generic_string()
	);
}

bool Configuration::Contains(const std::string_view& path) const {
	return TomlNode::Contains(config_, path); //!< 設定が存在するか.
}

const toml::table& Configuration::GetConfig(const std::string_view& path) const {
	if (!TomlNode::Contains(config_, path)) {
		static const toml::table empty{};
		return empty; //!< 設定が存在しない.
	}

	const auto node = TomlNode::GetNode(config_, path); //!< 設定のnodeの取得.
	STREAM_ASSERT(node->is_table(), "Configuration | config is not a table: {}", path);
	//!< 設定がテーブルでない.

	return *node->as_table(); //!< 設定のテーブルの取得.
}


