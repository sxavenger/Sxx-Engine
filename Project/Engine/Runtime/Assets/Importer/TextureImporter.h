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

	////////////////////////////////////////////////////////////////////////////////////////////
	// ReferenceParameters structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ReferenceParameters {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Graphics::EncodingColorSpace encoding = Graphics::EncodingColorSpace::Lightness; //!< カラースペースの指定
		bool isGenerateMipmap                 = true; //!< ミップマップを生成するかどうかの指定

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static Uuid Import(const std::filesystem::path& filepath, const ReferenceParameters& parameters);

};

SXAVENGER_ENGINE_NAMESPACE_END
