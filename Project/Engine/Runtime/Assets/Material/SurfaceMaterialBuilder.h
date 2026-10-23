#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "SurfaceMaterialMetadata.h"
#include "SurfaceMaterial.h"

//* assimp
#include "../Assimp/AssimpOption.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/GraphicsUtil.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class AssimpMaterial;

////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceMaterialBuilder class
////////////////////////////////////////////////////////////////////////////////////////////
class SurfaceMaterialBuilder final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Build(std::shared_ptr<SurfaceMaterial>& material);

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* build helper methods *//

	static SurfaceMaterial::Description BuildReference(const std::filesystem::path& directory, const SurfaceMaterialMetadata::ReferenceData& data);

	//* reference build methods *//

	static Graphics::EncodingColorSpace GetTextureEncoding(AssimpTextureType type);

	static std::optional<Uuid> GetTextureId(const AssimpMaterial& material, const std::filesystem::path& directory, AssimpTextureType type);

	static std::optional<Uuid> Unify(const std::optional<Uuid>& lhs, const std::optional<Uuid>& rhs);

};

SXAVENGER_ENGINE_NAMESPACE_END
