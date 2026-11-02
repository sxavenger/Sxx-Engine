#include "MainBuffer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

//=========================================================================================
// constant variables
//=========================================================================================

const ArrayEnum<DXGI_FORMAT, EnumUtil<MainBuffer::Layout>::GetCount(), MainBuffer::Layout> MainBuffer::kFormats = {
	DXGI_FORMAT_R16G16B16A16_FLOAT, //!< SceneRender
	DXGI_FORMAT_R16G16B16A16_FLOAT, //!< SceneOutput
};

////////////////////////////////////////////////////////////////////////////////////////////
// MainBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MainBuffer::Create(const Resolution& resolution) {
	{ //!< SceneRenderの生成

		//!< optionsの設定
		RenderTargetTexture::Options options = {};
		options.resolution = resolution.render; //!< upscale前の解像度を設定する.
		options.format     = kFormats[Layout::SceneRender];

		//!< textureの生成
		buffers_[Layout::SceneRender] = RenderTargetTexture::Create(options);
	}

	{ //!< SceneOutputの生成

		//!< optionsの設定
		RenderTargetTexture::Options options = {};
		options.resolution = resolution.output; //!< upscale後の解像度を設定する.
		options.format     = kFormats[Layout::SceneOutput];

		//!< textureの生成
		buffers_[Layout::SceneOutput] = RenderTargetTexture::Create(options);
	}


	//!< 名前の設定
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {
		buffers_[i].SetName(EnumUtil<Layout>::GetName(i));
	}
}

void MainBuffer::Resize(const Resolution& resolution) {
	buffers_[Layout::SceneRender].Resize(resolution.render); //!< SceneRenderはupscale前の解像度でリサイズする.
	buffers_[Layout::SceneOutput].Resize(resolution.output); //!< SceneOutputはupscale後の解像度でリサイズする.
}

void MainBuffer::ClearTarget(Graphics::GraphicsCommandContext& command) {
	for (EnumUtil<Layout>::Underlying i = 0; i < EnumUtil<Layout>::GetCount(); ++i) {
		D3D12_RESOURCE_STATES state = buffers_[i].GetCurrentState();
		buffers_[i].ClearRenderTarget(command);
		buffers_[i].Transition(command, state); //!< 元のresource状態に戻す.
	}
}
