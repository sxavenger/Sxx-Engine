#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/AssetTraits.h"
#include "SkeletalAnimation.h"
#include "SkeletalAnimationBuilder.h"

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
struct AssetTraits<SkeletalAnimation> final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Build(std::shared_ptr<SkeletalAnimation>& animation) { SkeletalAnimationBuilder::Build(animation); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	static constexpr AssetType kType = AssetType::SkeletalAnimation;

};

SXAVENGER_ENGINE_NAMESPACE_END
