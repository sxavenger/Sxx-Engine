#include "DepthStencilBuffer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

//=========================================================================================
// constant variables
//=========================================================================================

const ArrayEnum<DXGI_FORMAT, EnumUtil<DepthStencilBuffer::Layout>::GetCount(), DepthStencilBuffer::Layout> DepthStencilBuffer::kFormats = {
	DXGI_FORMAT_D24_UNORM_S8_UINT,
};

////////////////////////////////////////////////////////////////////////////////////////////
// DepthStencilBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DepthStencilBuffer::Create(const Vector2u& resolution) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {

		//!< optionsの設定
		DepthStencilTexture::Options options = {};
		options.resolution = resolution;
		options.format     = kFormats[i];

		//!< textureの生成
		buffers_[i] = DepthStencilTexture::Create(options);
		buffers_[i].SetName(EnumUtil<Layout>::GetName(i));
	}
}

void DepthStencilBuffer::Resize(const Vector2u& resolution) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {
		buffers_[i].Resize(resolution);
	}
}

void DepthStencilBuffer::ClearTarget(Graphics::GraphicsCommandContext& command) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {
		buffers_[i].ClearDepthStencil(command);
	}
}
