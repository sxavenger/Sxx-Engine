#include "LightingBuffer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

//=========================================================================================
// constant variables
//=========================================================================================

const ArrayEnum<DXGI_FORMAT, EnumUtil<LightingBuffer::Layout>::GetCount(), LightingBuffer::Layout> LightingBuffer::kFormats = {
	DXGI_FORMAT_R16G16B16A16_FLOAT, //!< Direct
	DXGI_FORMAT_R16G16B16A16_FLOAT, //!< Indirect
};

////////////////////////////////////////////////////////////////////////////////////////////
// LightingBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void LightingBuffer::Create(const Resolution& resolution) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {

		//!< optionsの設定
		RenderTargetTexture::Options options = {};
		options.resolution = resolution.render;
		options.format = kFormats[i];

		//!< textureの生成
		buffers_[i] = RenderTargetTexture::Create(options);
		buffers_[i].SetName(EnumUtil<Layout>::GetName(i));
	}
}

void LightingBuffer::Resize(const Resolution& resolution) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {
		buffers_[i].Resize(resolution.render);
	}
}

void LightingBuffer::ClearTarget(Graphics::GraphicsCommandContext& command) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {
		D3D12_RESOURCE_STATES state = buffers_[i].GetCurrentState();
		buffers_[i].ClearRenderTarget(command);
		buffers_[i].Transition(command, state);
	}
}
