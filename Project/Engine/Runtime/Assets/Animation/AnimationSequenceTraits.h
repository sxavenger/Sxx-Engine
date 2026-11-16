#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/AssetTraits.h"
#include "AnimationSequence.h"
#include "AnimationSequenceBuilder.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssetTraits<StaticMesh> structure
////////////////////////////////////////////////////////////////////////////////////////////
template <>
struct AssetTraits<AnimationSequence> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Build(std::shared_ptr<AnimationSequence>& animation) { AnimationSequenceBuilder::Build(animation); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	static constexpr AssetType kType = AssetType::AnimationSequence;

};

SXAVENGER_ENGINE_NAMESPACE_END
