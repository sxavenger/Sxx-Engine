#include "RenderProcessBuffer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//=========================================================================================
// constant variables
//=========================================================================================

const DXGI_FORMAT RenderProcessBuffer::kFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

////////////////////////////////////////////////////////////////////////////////////////////
// RenderProcessBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderProcessBuffer::Create(const Resolution& resolution) {
	for (size_t i = 0; i < buffers_.size(); ++i) {

		//!< optionsの設定
		RenderTargetTexture::Options options = {};
		options.resolution = resolution.render;
		options.format     = kFormat;

		//!< textureの生成
		buffers_[i] = RenderTargetTexture::Create(options);
		buffers_[i].SetName(std::format("RenderXXXBuffer [{}]", i));
	}
}

void RenderProcessBuffer::Resize(const Resolution& resolution) {
	for (size_t i = 0; i < buffers_.size(); ++i) {
		buffers_[i].Resize(resolution.render);
		buffers_[i].SetName(std::format("RenderProcessBuffer [{}]", i));
	}
}

void RenderProcessBuffer::ClearTarget(Graphics::GraphicsCommandContext& command) {
	for (size_t i = 0; i < buffers_.size(); ++i) {
		D3D12_RESOURCE_STATES state = buffers_[i].GetCurrentState();
		buffers_[i].ClearRenderTarget(command);
		buffers_[i].Transition(command, state); //!< 元のresource状態に戻す.
	}
}

void RenderProcessBuffer::IncrementBuffer() {
	currentBufferIndex_ = (currentBufferIndex_ + 1) % kBufferCount; //!< 次のbuffer indexに更新する.
}

RenderTargetTexture& RenderProcessBuffer::GetCurrentBuffer() {
	return buffers_[currentBufferIndex_];
}

const RenderTargetTexture& RenderProcessBuffer::GetCurrentBuffer() const {
	return buffers_[currentBufferIndex_];
}

RenderTargetTexture& RenderProcessBuffer::GetPreviousBuffer(uint8_t offset) {
	STREAM_ASSERT(offset < kBufferCount, "offset is out of range.");
	return buffers_[(currentBufferIndex_ + kBufferCount - offset) % kBufferCount];
}

const RenderTargetTexture& RenderProcessBuffer::GetPreviousBuffer(uint8_t offset) const {
	STREAM_ASSERT(offset < kBufferCount, "offset is out of range.");
	return buffers_[(currentBufferIndex_ + kBufferCount - offset) % kBufferCount];
}
