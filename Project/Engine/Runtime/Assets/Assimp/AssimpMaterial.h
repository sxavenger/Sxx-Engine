#pragma once

//-----------------------------------------------------------------------------------------
// include
//----------------------------------------------------------------------------------------
//* assets
#include "AssimpOption.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Math/Transform/Transform2d.h>
#include <Lib/Math/Color3.h>

//* assimp
#include <assimp/material.h>

//* c++
#include <string>
#include <optional>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpMaterial class
////////////////////////////////////////////////////////////////////////////////////////////
class AssimpMaterial final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	AssimpMaterial() = delete;
	AssimpMaterial(const aiMaterial* material) noexcept : material_(material) {}

	//* material option *//

	std::string GetName() const;

	//* texture option *//

	std::optional<std::filesystem::path> GetTexturePath(const std::filesystem::path& directory, AssimpTextureType type, uint32_t index = 0) const;

	//* transform option *//

	Transform2d GetTransform() const;

	//* value option *//

	Color3f GetDiffuse() const;

	Color3f GetEmissive() const;

	float GetRoughness() const;

	float GetMetallic() const;


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	RefPtr<const aiMaterial> material_ = nullptr;
	
};

SXAVENGER_ENGINE_NAMESPACE_END
