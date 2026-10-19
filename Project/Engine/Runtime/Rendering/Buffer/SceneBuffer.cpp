#include "SceneBuffer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

//=========================================================================================
// constant variables
//=========================================================================================

const ArrayEnum<DXGI_FORMAT, EnumUtil<SceneBuffer::Layout>::GetCount(), SceneBuffer::Layout> SceneBuffer::kFormats = {
	DXGI_FORMAT_R16G16B16A16_FLOAT, //!< Scene
};

////////////////////////////////////////////////////////////////////////////////////////////
// SceneBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneBuffer::Create(const Vector2u& resolution) {
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

void SceneBuffer::Resize(const Vector2u& resolution) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {
		buffers_[i].Resize(resolution);
	}
}

void SceneBuffer::ClearTarget(Graphics::GraphicsCommandContext& command) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {
		buffers_[i].ClearRenderTarget(command);
	}
}
