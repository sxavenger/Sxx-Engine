#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Uuid/Uuid.h>

//* c++
#include <vector>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// ModelContents structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ModelContents final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	const Uuid& GetMeshMaterialId(const Uuid& meshId) const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::vector<Uuid> meshIds;
	std::vector<Uuid> materialIds;
	Uuid boneId;

	std::unordered_map<Uuid, Uuid> meshMaterialMap; //!< meshに対応するmaterialのuuidを格納するunordered_map
};

SXAVENGER_ENGINE_NAMESPACE_END
