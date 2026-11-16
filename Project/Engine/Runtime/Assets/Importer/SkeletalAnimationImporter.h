#pragma once

#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../AssetUtil.h"
#include "../Contents/SkeletalAnimationContents.h"

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
// SkeletalAnimationImporter class
////////////////////////////////////////////////////////////////////////////////////////////
class SkeletalAnimationImporter final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ReferenceParameters structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ReferenceParameters {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t index = 0; //!< 参照するマテリアルのインデックス

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static SkeletalAnimationContents Import(const std::filesystem::path& filepath, const ReferenceParameters& parameters);

};

SXAVENGER_ENGINE_NAMESPACE_END

