#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "AnimationSequenceMetadata.h"
#include "AnimationSequence.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationSequenceBuilder class
////////////////////////////////////////////////////////////////////////////////////////////
class AnimationSequenceBuilder final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Build(std::shared_ptr<AnimationSequence>& animation);

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* build helper methods *//

	static AnimationSequence::Description BuildReference(const std::filesystem::path& directory, const AnimationSequenceMetadata::ReferenceData& data);

};

SXAVENGER_ENGINE_NAMESPACE_END
