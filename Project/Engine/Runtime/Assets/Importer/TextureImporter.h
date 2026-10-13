#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../AssetUtil.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/GraphicsUtil.h>

//* lib
#include <Lib/Uuid/Uuid.h>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// TextureImporter class
////////////////////////////////////////////////////////////////////////////////////////////
class TextureImporter final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static Uuid Import(const std::filesystem::path& filepath, Graphics::ColorEncoding encoding = Graphics::ColorEncoding::Lightness);

};

SXAVENGER_ENGINE_NAMESPACE_END
