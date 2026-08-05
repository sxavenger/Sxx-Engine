#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* json
#include "JsonUtil.h"

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// JsonFile class
////////////////////////////////////////////////////////////////////////////////////////////
class JsonFile final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* load option *//

	static json::node Load(const std::filesystem::path& filepath);

	static bool TryLoad(const std::filesystem::path& filepath, json::node& out);

	//* write option *//

	static void Write(const std::filesystem::path& filepath, const json::node& node);

};
