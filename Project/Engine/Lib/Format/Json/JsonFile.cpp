#include "JsonFile.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <fstream>

////////////////////////////////////////////////////////////////////////////////////////////
// JsonFile class methods
////////////////////////////////////////////////////////////////////////////////////////////

json::node JsonFile::Load(const std::filesystem::path& filepath) {
	STREAM_ASSERT(std::filesystem::exists(filepath), "json file not found. path: {}", filepath.generic_string());
	return json::node::parse(std::ifstream(filepath), nullptr, true, true); //!< json with comment(.jsonc)の読み込みを可能に.
}

bool JsonFile::TryLoad(const std::filesystem::path& filepath, json::node& out) {
	if (!std::filesystem::exists(filepath)) {
		return false;
	}

	out = json::node::parse(std::ifstream(filepath), nullptr, true, true); //!< json with comment(.jsonc)の読み込みを可能に.
	return true;
}

void JsonFile::Write(const std::filesystem::path& filepath, const json::node& node) {
	std::ofstream ofs(filepath);
	STREAM_ASSERT(ofs.is_open(), "json file open error. path: {}", filepath.generic_string());

	ofs << std::setfill('\t') << std::setw(1) << node << std::endl;
}
