#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "SkeletalAnimationMetadata.h"
#include "SkeletalAnimation.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletalAnimationBuilder class
////////////////////////////////////////////////////////////////////////////////////////////
class SkeletalAnimationBuilder final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Build(std::shared_ptr<SkeletalAnimation>& animation);

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* build helper methods *//

	static SkeletalAnimation::Description BuildReference(const std::filesystem::path& directory, const SkeletalAnimationMetadata::ReferenceData& data);

};

SXAVENGER_ENGINE_NAMESPACE_END
