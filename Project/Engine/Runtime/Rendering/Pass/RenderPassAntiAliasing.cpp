#include "RenderPassAntiAliasing.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "../Buffer/MainBuffer.h"
#include "../Buffer/RenderProcessBuffer.h"

//* engine
#include <Runtime/Graphics/Core.h>

//* lib
#include <Lib/Logger/StackLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPassAntiAliasing class methods
////////////////////////////////////////////////////////////////////////////////////////////

RenderPassAntiAliasing::RenderPassAntiAliasing() {

	//!< 共通Samplerの設定
	Graphics::StaticSamplerSet samplers = {};
	samplers.SetSamplerFilter("gSamplerLinearClamp", Graphics::SampleFilter::Linear, Graphics::SampleMode::Clamp);

	{ //!< FXAA Anti-Aliasing Compute Pipeline
		Graphics::ComputePipelineState::Desc desc = {};
		desc.SetShaderBlob(Graphics::Core::CompileShader(L"Engine/Packages/shaders/RenderPass/AntiAliasing/FXAA/FXAA.cs.hlsl", Graphics::CompileProfile::Compute, L"main"));

		pipelines_[Pipeline::FXAA] = Graphics::ReflectedComputePipelineState::Create(
			Graphics::Core::GetDevice(),
			desc,
			samplers
		);
	}
}

void RenderPassAntiAliasing::Setup(MAYBE_UNUSED const PassContext& context) {
	//!< bufferの確保
	context.buffer->EnsureInstance<RenderProcessBuffer>(context.GetBufferResolution());
}

void RenderPassAntiAliasing::Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) {

	if (context.option.antiAliasing == PassOption::AntiAliasing::None) {
		return; //!< AntiAliasingが無効の場合, 実行しない.
	}

	if (!context.buffer->HasInstance<MainBuffer>()) {
		STACK_LOG_WARNING("Rendering::RenderPassAntiAliasing | <MainBuffer> is not found.");
		return; //!< Bufferが存在しない場合, 実行しない.
	}

	BaseRenderPass::BeginEvent(command, "Anti-Aliasing");

	{ //!< Anti-Aliasing Pass

		BaseRenderPass::BeginEvent(command, std::format("Anti-Aliasing Pass | {}", context.option.antiAliasing));

		BeginAntiAliasingPass(command, *context.buffer);

		switch (context.option.antiAliasing) {
			case PassOption::AntiAliasing::FXAA:
				AntiAliasingFXAA(command, context, context.GetContextData());
				break;
		}

		EndAntiAliasingPass(command, *context.buffer);

		BaseRenderPass::EndEvent(command);
	}

	BaseRenderPass::EndEvent(command);

}

void RenderPassAntiAliasing::BeginAntiAliasingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

	auto commandList = command.GetCommandList();

	//!< bufferの取得
	MainBuffer& main             = buffer.GetInstance<MainBuffer>();
	RenderProcessBuffer& process = buffer.GetInstance<RenderProcessBuffer>();

	RenderTargetTexture& input  = main.GetBuffer(MainBuffer::Layout::SceneRender);
	RenderTargetTexture& output = process.GetCurrentBuffer();

	{ //!< copyの実行

		input.Transition(command, RenderTargetTexture::State::Source);
		output.Transition(command, RenderTargetTexture::State::Dest);

		commandList->CopyResource(output.GetResource().Get(), input.GetResource().Get());
	}

	{ //!< barrierの設定

		//!< RenderXXXBufferをDefaultに遷移
		output.Transition(command, RenderTargetTexture::State::Default);

		//!< MainBufferをDefaultに遷移
		input.Transition(command, RenderTargetTexture::State::Default);
	}
}

void RenderPassAntiAliasing::EndAntiAliasingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

	auto commandList = command.GetCommandList();

	//!< bufferの取得
	MainBuffer& main             = buffer.GetInstance<MainBuffer>();
	RenderProcessBuffer& process = buffer.GetInstance<RenderProcessBuffer>();

	RenderTargetTexture& input  = process.GetCurrentBuffer();
	RenderTargetTexture& output = main.GetBuffer(MainBuffer::Layout::SceneRender);

	{ //!< copyの実行
		input.Transition(command, RenderTargetTexture::State::Source);
		output.Transition(command, RenderTargetTexture::State::Dest);

		commandList->CopyResource(output.GetResource().Get(), input.GetResource().Get());
	}

	{ //!< barrierの設定

		//!< MainBufferをUnorderedAccessに遷移
		output.Transition(command, RenderTargetTexture::State::Default);

		//!< RenderXXXBufferを通常状態に遷移
		input.Transition(command, RenderTargetTexture::State::Default);
	}
}

void RenderPassAntiAliasing::AntiAliasingFXAA(Graphics::GraphicsCommandContext& command, const PassContext& context, const PassContext::ContextData& data) {

	auto commandList = command.GetCommandList();

	//!< bufferの取得
	RenderProcessBuffer& process = context.buffer->GetInstance<RenderProcessBuffer>();
	process.IncrementBuffer();
	process.GetCurrentBuffer().Transition(command, RenderTargetTexture::State::UnorderedAccess);

	const auto& pipeline = pipelines_[Pipeline::FXAA];

	pipeline.BindPipeline(command);

	Graphics::ShaderParameter parameter;

	//!< 共通parameterの設定
	parameter.Set32bitConstants("Context", data);
	parameter.Set32bitConstants("PassData", data_);

	//!< Input情報の設定
	parameter.SetHandle("gInput", process.GetPreviousBuffer().GetDescriptorSRV().GetGPUHandle());

	//!< Output情報の設定
	parameter.SetHandle("gOutput", process.GetCurrentBuffer().GetDescriptorUAV().GetGPUHandle());

	pipeline.BindShaderParameter(command, parameter);

	const Vector3u& threadGroupSize = pipeline.GetThreadGroupSize();
	commandList->Dispatch(
		Graphics::RoundUp(data.renderResolution.x, threadGroupSize.x),
		Graphics::RoundUp(data.renderResolution.y, threadGroupSize.y),
		1
	);


}
