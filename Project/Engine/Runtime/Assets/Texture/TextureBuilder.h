#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "TextureMetadata.h"
#include "Texture.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Core/GraphicsCommandContext.h>
#include <Runtime/Graphics/Buffer/Resource.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// TextureBuilder class
////////////////////////////////////////////////////////////////////////////////////////////
class TextureBuilder final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Build(std::shared_ptr<Texture>& texture);

private:
	
	//=========================================================================================
	// private methods
	//=========================================================================================

	//* load helper methods *//

	static DirectX::ScratchImage GenerateMipmaps(const DirectX::ScratchImage& source, DirectX::TEX_FILTER_FLAGS flags);

	//* texture load methods *//

	static DirectX::ScratchImage LoadTextureFromDDS(const std::filesystem::path& directory, const TextureMetadata::ReferenceData& reference);
	static DirectX::ScratchImage LoadTextureFromHDR(const std::filesystem::path& directory, const TextureMetadata::ReferenceData& reference);
	static DirectX::ScratchImage LoadTextureFromTGA(const std::filesystem::path& directory, const TextureMetadata::ReferenceData& reference);
	static DirectX::ScratchImage LoadTextureFromWIC(const std::filesystem::path& directory, const TextureMetadata::ReferenceData& reference);
	static DirectX::ScratchImage LoadTextureFile(const std::filesystem::path& directory, const TextureMetadata::ReferenceData& reference);

};

SXAVENGER_ENGINE_NAMESPACE_END
