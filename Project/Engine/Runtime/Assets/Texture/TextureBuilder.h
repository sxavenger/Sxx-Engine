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

	// vvv 廃止予定(GPU側のResource管理は別のクラスに任せる) vvv //

	//* resource methods *//

	DEPRECATED("Use GPU-side resource management instead")
	static Graphics::Resource CreateTextureResource(const std::string_view& name, const Texture::Description& description);

	DEPRECATED("Use GPU-side resource management instead")
	NODISCARD static Graphics::Resource UploadResourceData(const Graphics::GraphicsCommandContext& context, const Graphics::Resource& resource, const DirectX::ScratchImage& image);

	//* descriptor methods *//

	DEPRECATED("Use GPU-side resource management instead")
	static void CreateDescriptor(Graphics::Descriptor& descriptor, const Graphics::Resource& resource, const Texture::Description& description);

};

SXAVENGER_ENGINE_NAMESPACE_END
