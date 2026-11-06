#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Math/Transform/Transform3d.h>

//* assimp
#include <assimp/scene.h>

//* c++
#include <string_view>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpNode class
////////////////////////////////////////////////////////////////////////////////////////////
class AssimpNode final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	AssimpNode() = delete;
	AssimpNode(const aiNode* node) noexcept : node_(node) {}

	//* node option *//

	std::string_view GetName() const;

	uint32_t GetChildCount() const;

	AssimpNode GetChild(uint32_t index) const;

	uint32_t GetMeshCount() const;

	uint32_t GetMeshIndex(uint32_t index) const;

	Transform3d GetTransform() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	RefPtr<const aiNode> node_; //!< assimpのノードへの参照

};

SXAVENGER_ENGINE_NAMESPACE_END
