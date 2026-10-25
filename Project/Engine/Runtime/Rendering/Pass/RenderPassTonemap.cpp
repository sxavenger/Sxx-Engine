#include "RenderPassTonemap.h"
SXAVENGER_ENGINE_USING_(Rendering)

#define DEBUG_TONEMAP 1

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "../Buffer/LightingBuffer.h" //!< todo: main bufferに変更する.

//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPassTonemap class methods
////////////////////////////////////////////////////////////////////////////////////////////

RenderPassTonemap::RenderPassTonemap() {

	{ //!< Tonemap Compute Pipeline
		Sxx::Graphics::ComputePipelineState::Desc desc = {};
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/RenderPass/Tonemap/Tonemap.cs.hlsl", Sxx::Graphics::CompileProfile::Compute, L"main"));

		pipeline_ = Graphics::ReflectedComputePipelineState::Create(
			Graphics::Core::GetDevice(),
			desc
		);
	}

}

void RenderPassTonemap::Setup(const PassContext& context) {
	//!< bufferの確保
	context.buffer->EnsureInstance<LightingBuffer>(context.resolution);
}

void RenderPassTonemap::Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) {

	BaseRenderPass::BeginEvent(command, "Tonemap");

	{ //!< Tonemap Pass
		BaseRenderPass::BeginEvent(command, "Tonemap Pass");

		BeginTonemapPass(command, *context.buffer);

		Tonemap(command, context, context.GetContextData());

		EndTonemapPass(command, *context.buffer);

		BaseRenderPass::EndEvent(command);
	}

	BaseRenderPass::EndEvent(command);

}

void RenderPassTonemap::BeginTonemapPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {


	auto commandList = command.GetCommandList();

	//!< bufferの取得
	LightingBuffer& lighting = buffer.GetInstance<LightingBuffer>();

	static const size_t kBufferCount = 1;
	ArrayEnum<RenderTargetTexture*, kBufferCount, LightingBuffer::Layout> buffers = {
		&lighting.GetBuffer(LightingBuffer::Layout::Direct),
	};

	{ //!< barrierの設定

		//!< LightingBufferをUnorderedAccessに遷移
		for (EnumUtil<LightingBuffer::Layout>::Underlying i = 0; i < kBufferCount; ++i) {
			buffers[i]->Transition(command, RenderTargetTexture::State::UnorderedAccess);
		}
	}

}

void RenderPassTonemap::EndTonemapPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

	auto commandList = command.GetCommandList();

	//!< bufferの取得
	LightingBuffer& lighting = buffer.GetInstance<LightingBuffer>();

	static const size_t kBufferCount = 1;
	ArrayEnum<RenderTargetTexture*, kBufferCount, LightingBuffer::Layout> buffers = {
		&lighting.GetBuffer(LightingBuffer::Layout::Direct),
	};

	{ //!< barrierの設定

		//!< LightingBufferを通常状態に遷移
		for (EnumUtil<LightingBuffer::Layout>::Underlying i = 0; i < kBufferCount; ++i) {
			buffers[i]->Transition(command, RenderTargetTexture::State::Default);
		}
	}
}

void RenderPassTonemap::Tonemap(Graphics::GraphicsCommandContext& command, const PassContext& context, const PassContext::ContextData& data) {

	auto commandList = command.GetCommandList();

	pipeline_.BindPipeline(command);

	Graphics::ShaderParameter parameter;

	//!< 共通parameterの設定
	parameter.Set32bitConstants("Context", data);
	parameter.Set32bitConstants("PassData", data_);

	//!< Color情報の設定
	parameter.SetHandle("gSceneColor", context.buffer->GetInstance<LightingBuffer>().GetBuffer(LightingBuffer::Layout::Direct).GetDescriptorUAV().GetGPUHandle());

	pipeline_.BindShaderParameter(command, parameter);

	const Vector3u& threadGroupSize = pipeline_.GetThreadGroupSize();
	commandList->Dispatch(
		Graphics::RoundUp(context.resolution.x, threadGroupSize.x),
		Graphics::RoundUp(context.resolution.y, threadGroupSize.y),
		1
	);
}
