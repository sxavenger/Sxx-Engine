#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "SkeletalMeshMetadata.h"
#include "SkeletalMesh.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletalMeshBuilder class
////////////////////////////////////////////////////////////////////////////////////////////
class SkeletalMeshBuilder final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Build(std::shared_ptr<SkeletalMesh>& mesh);

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* build helper methods *//

	static SkeletalMesh::Description BuildReference(const std::filesystem::path& directory, const SkeletalMeshMetadata::ReferenceData& data);

};

SXAVENGER_ENGINE_NAMESPACE_END
