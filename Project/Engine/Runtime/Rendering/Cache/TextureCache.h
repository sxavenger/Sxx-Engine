#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Core/GraphicsCommandContext.h>
#include <Runtime/Graphics/Core/Descriptor.h>
#include <Runtime/Graphics/Buffer/ResourceHandle.h>
#include <Runtime/Graphics/Buffer/Resource.h>
#include <Runtime/Assets/Texture/Texture.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// TextureCache class
////////////////////////////////////////////////////////////////////////////////////////////
class TextureCache final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Cache(const std::shared_ptr<Assets::Texture>& texture);

	//* address option *//

	uintptr_t GetAddress() const { return address_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	Graphics::ResourceHandle handle_;
	Graphics::Descriptor descriptor_;

	//* address *//

	uintptr_t address_ = NULL;

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
