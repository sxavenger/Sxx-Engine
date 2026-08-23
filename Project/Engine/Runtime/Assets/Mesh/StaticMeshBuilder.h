#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "StaticMeshMetadata.h"
#include "StaticMesh.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// StaticMeshBuilder class
////////////////////////////////////////////////////////////////////////////////////////////
class StaticMeshBuilder final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Build(std::shared_ptr<StaticMesh>& mesh);

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* build helper methods *//

	static void BuildReference(StaticMesh::Description& description, const std::filesystem::path& directory, const StaticMeshMetadata::ReferenceData& data);

	static void Optimize(StaticMesh::Description& description);

};

SXAVENGER_ENGINE_NAMESPACE_END
