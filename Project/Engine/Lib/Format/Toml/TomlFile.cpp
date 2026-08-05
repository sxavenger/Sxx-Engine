#include "TomlFile.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <format>
#include <fstream>

////////////////////////////////////////////////////////////////////////////////////////////
// TomlFile class methods
////////////////////////////////////////////////////////////////////////////////////////////

toml::table TomlFile::Load(const std::filesystem::path& filepath) {
	StreamLogger::Assert(std::filesystem::exists(filepath), std::format("toml file not found. filepath: {}", filepath.generic_string()));
	return toml::parse_file(filepath.generic_wstring());
}

bool TomlFile::TryLoad(const std::filesystem::path& filepath, toml::table& out) {
	if (!std::filesystem::exists(filepath)) {
		return false;
	}

	out = toml::parse_file(filepath.generic_wstring());
	return true;
}
