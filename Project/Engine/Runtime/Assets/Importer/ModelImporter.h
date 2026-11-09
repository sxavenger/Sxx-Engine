#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../AssetUtil.h"
#include "../Contents/ModelContents.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Uuid/Uuid.h>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// ModelImporter class
////////////////////////////////////////////////////////////////////////////////////////////
class ModelImporter final {
public:
	
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static ModelContents Import(const std::filesystem::path& filepath);

};

SXAVENGER_ENGINE_NAMESPACE_END
