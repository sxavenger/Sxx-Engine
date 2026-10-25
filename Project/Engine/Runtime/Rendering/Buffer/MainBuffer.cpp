#include "MainBuffer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

//=========================================================================================
// constant variables
//=========================================================================================

const ArrayEnum<DXGI_FORMAT, EnumUtil<MainBuffer::Layout>::GetCount(), MainBuffer::Layout> MainBuffer::kFormats = {
	DXGI_FORMAT_R16G16B16A16_FLOAT, //!< Scene
};

////////////////////////////////////////////////////////////////////////////////////////////
// MainBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MainBuffer::Create(const Vector2u& resolution) {
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

void MainBuffer::Resize(const Vector2u& resolution) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {
		buffers_[i].Resize(resolution);
	}
}

void MainBuffer::ClearTarget(Graphics::GraphicsCommandContext& command) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {
		buffers_[i].ClearRenderTarget(command);
	}
}
