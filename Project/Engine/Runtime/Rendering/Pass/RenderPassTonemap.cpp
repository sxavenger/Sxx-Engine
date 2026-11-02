#include "RenderPassTonemap.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "../Buffer/MainBuffer.h"

//* engine
#include <Runtime/Graphics/Core.h>

//* lib
#include <Lib/Logger/StackLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPassTonemap class methods
////////////////////////////////////////////////////////////////////////////////////////////

RenderPassTonemap::RenderPassTonemap() {

	{ //!< Tonemap Compute Pipeline
		Graphics::ComputePipelineState::Desc desc = {};
		desc.SetShaderBlob(Graphics::Core::CompileShader(L"Engine/Packages/shaders/RenderPass/Tonemap/Tonemap.cs.hlsl", Graphics::CompileProfile::Compute, L"main"));

		pipeline_ = Graphics::ReflectedComputePipelineState::Create(
			Graphics::Core::GetDevice(),
			desc
		);
	}

}

void RenderPassTonemap::Setup(MAYBE_UNUSED const PassContext& context) {
}

void RenderPassTonemap::Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) {

	if (!context.buffer->HasInstance<MainBuffer>()) {
		STACK_LOG_WARNING("Rendering::RenderPassTonemap | <MainBuffer> is not found.");
		return; //!< Bufferが存在しない場合, 実行しない.
	}

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
	MainBuffer& main = buffer.GetInstance<MainBuffer>();

	static const size_t kBufferCount = 1;
	ArrayEnum<RenderTargetTexture*, kBufferCount, MainBuffer::Layout> buffers = {
		&main.GetBuffer(MainBuffer::Layout::SceneOutput),
	};

	{ //!< barrierの設定


		//!< MainBufferをUnorderedAccessに遷移
		for (EnumUtil<MainBuffer::Layout>::Underlying i = 0; i < kBufferCount; ++i) {
			buffers[i]->Transition(command, RenderTargetTexture::State::UnorderedAccess);
		}
	}

}

void RenderPassTonemap::EndTonemapPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

	auto commandList = command.GetCommandList();

	//!< bufferの取得
	MainBuffer& main = buffer.GetInstance<MainBuffer>();

	static const size_t kBufferCount = 1;
	ArrayEnum<RenderTargetTexture*, kBufferCount, MainBuffer::Layout> buffers = {
		&main.GetBuffer(MainBuffer::Layout::SceneOutput),
	};

	{ //!< barrierの設定

		//!< MainBufferを通常状態に遷移
		for (EnumUtil<MainBuffer::Layout>::Underlying i = 0; i < kBufferCount; ++i) {
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
	parameter.SetHandle("gSceneOutput", context.buffer->GetInstance<MainBuffer>().GetBuffer(MainBuffer::Layout::SceneOutput).GetDescriptorUAV().GetGPUHandle());

	pipeline_.BindShaderParameter(command, parameter);

	const Vector3u& threadGroupSize = pipeline_.GetThreadGroupSize();
	commandList->Dispatch(
		Graphics::RoundUp(data.outputResolution.x, threadGroupSize.x),
		Graphics::RoundUp(data.outputResolution.y, threadGroupSize.y),
		1
	);
}
