#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cache
#include "BaseStaticCache.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Descriptor/Descriptor.h>
#include <Runtime/Graphics/Command/GraphicsCommandContext.h>
#include <Runtime/Graphics/Buffer/ResourceHandle.h>
#include <Runtime/Graphics/Buffer/Resource.h>
#include <Runtime/Assets/Texture/Texture.h>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// TextureCache class
////////////////////////////////////////////////////////////////////////////////////////////
class TextureCache final
	: public BaseStaticCache {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Cache(const std::shared_ptr<Assets::Texture>& texture);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* Graphics *//

	Graphics::ResourceHandle handle_;
	Graphics::Descriptor descriptor_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* resource methods *//

	static Graphics::ResourceHandle CreateTextureResource(const std::string_view& name, const Assets::Texture::Description& description);

	NODISCARD static Graphics::Resource UploadResourceData(const Graphics::GraphicsCommandContext& context, const Graphics::Resource& resource, const DirectX::ScratchImage& image);

	//* descriptor methods *//

	static void CreateDescriptor(Graphics::Descriptor& descriptor, const Graphics::Resource& resource, const Assets::Texture::Description& description);

};

SXAVENGER_ENGINE_NAMESPACE_END
