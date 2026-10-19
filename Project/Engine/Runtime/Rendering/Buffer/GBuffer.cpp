#include "GBuffer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

//=========================================================================================
// constant variables
//=========================================================================================

const ArrayEnum<DXGI_FORMAT, EnumUtil<GBuffer::Layout>::GetCount(), GBuffer::Layout> GBuffer::kFormats = {
	DXGI_FORMAT_R8G8B8A8_UNORM,    //!< GBufferA
	DXGI_FORMAT_R10G10B10A2_UNORM, //!< GBufferB
	DXGI_FORMAT_R8G8B8A8_UNORM,    //!< GBufferC
};

////////////////////////////////////////////////////////////////////////////////////////////
// GBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GBuffer::Create(const Vector2u& resolution) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {

		//!< optionsの設定
		RenderTargetTexture::Options options = {};
		options.resolution = resolution;
		options.format     = kFormats[i];

		//!< textureの生成
		buffers_[i] = RenderTargetTexture::Create(options);
		buffers_[i].SetName(EnumUtil<Layout>::GetName(i));
	}
}

void GBuffer::Resize(const Vector2u& resolution) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {
		buffers_[i].Resize(resolution);
	}
}

void GBuffer::ClearTarget(Graphics::GraphicsCommandContext& command) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {
		buffers_[i].ClearRenderTarget(command);
	}
}
