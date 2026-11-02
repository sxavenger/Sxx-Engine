#include "RenderPassUpscaling.h"
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
// RenderPassUpscaling class methods
////////////////////////////////////////////////////////////////////////////////////////////

RenderPassUpscaling::RenderPassUpscaling() {

	//!< 共通Samplerの設定
	Graphics::StaticSamplerSet samplers = {};
	samplers.SetSamplerFilter("gSamplerLinearClamp", Graphics::SampleFilter::Linear, Graphics::SampleMode::Clamp);

	{ //!< Bilinear Upscaling Compute Pipeline
		Graphics::ComputePipelineState::Desc desc = {};
		desc.SetShaderBlob(Graphics::Core::CompileShader(L"Engine/Packages/shaders/RenderPass/Upscaling/Bilinear/Bilinear.cs.hlsl", Graphics::CompileProfile::Compute, L"main"));

		pipelines_[Pipeline::Bilinear] = Graphics::ReflectedComputePipelineState::Create(
			Graphics::Core::GetDevice(),
			desc,
			samplers
		);
	}

	{ //!< NIS Upscaling Compute Pipeline
		Graphics::ComputePipelineState::Desc desc = {};
		desc.SetShaderBlob(Graphics::Core::CompileShader(L"Engine/Packages/shaders/RenderPass/Upscaling/NIS/NIS.cs.hlsl", Graphics::CompileProfile::Compute, L"main"));

		pipelines_[Pipeline::NIS] = Graphics::ReflectedComputePipelineState::Create(
			Graphics::Core::GetDevice(),
			desc,
			samplers
		);
	}
}

void RenderPassUpscaling::Setup(MAYBE_UNUSED const PassContext& context) {

	//!< upscalerの設定
	switch (context.option.upscaler) {
		case PassOption::Upscaler::NIS: //!< NIS Upscaling
			upscalerNIS_.Setup();
			break;
	}
}

void RenderPassUpscaling::Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) {

	if (!context.buffer->HasInstance<MainBuffer>()) {
		STACK_LOG_WARNING("Rendering::RenderPassUpscaling | <MainBuffer> is not found.");
		return; //!< Bufferが存在しない場合, 実行しない.
	}

	BaseRenderPass::BeginEvent(command, "Upscaling");

	{ //!< Upscaling Pass
		BaseRenderPass::BeginEvent(command, std::format("Upscaling Pass | {} - {}", context.option.upscaler, context.option.upscalerMode));

		BeginUpscalingPass(command, *context.buffer);

		switch (context.option.upscaler) {
			case PassOption::Upscaler::Bilinear: //!< Bilinear Upscaling
				UpscalingBilinear(command, context, context.GetContextData());
				break;

			case PassOption::Upscaler::NIS: //!< NIS Upscaling
				UpscalingNIS(command, context, context.GetContextData());
				break;
		}

		EndUpscalingPass(command, *context.buffer);

		BaseRenderPass::EndEvent(command);
	}

	BaseRenderPass::EndEvent(command);

}

void RenderPassUpscaling::BeginUpscalingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

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

void RenderPassUpscaling::EndUpscalingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

	auto commandList = command.GetCommandList();

	//!< bufferの取得
	MainBuffer& main = buffer.GetInstance<MainBuffer>();

	static const size_t kBufferCount = 1;
	ArrayEnum<RenderTargetTexture*, kBufferCount, MainBuffer::Layout> buffers = {
		&main.GetBuffer(MainBuffer::Layout::SceneOutput),
	};

	{ //!< barrierの設定

		//!< MainBufferをUnorderedAccessからShaderResourceに遷移
		for (EnumUtil<MainBuffer::Layout>::Underlying i = 0; i < kBufferCount; ++i) {
			buffers[i]->Transition(command, RenderTargetTexture::State::Default);
		}
	}

}

void RenderPassUpscaling::UpscalingBilinear(Graphics::GraphicsCommandContext& command, const PassContext& context, const PassContext::ContextData& data) {

	auto commandList     = command.GetCommandList();
	const auto& pipeline = pipelines_[Pipeline::Bilinear];

	pipeline.BindPipeline(command);

	Graphics::ShaderParameter parameter;

	//!< 共通parameterの設定
	parameter.Set32bitConstants("Context", data);
	parameter.Set32bitConstants("PassData", data_);

	//!< Render情報の設定
	parameter.SetHandle("gSceneRender", context.buffer->GetInstance<MainBuffer>().GetBuffer(MainBuffer::Layout::SceneRender).GetDescriptorSRV().GetGPUHandle());

	//!< Output情報の設定
	parameter.SetHandle("gSceneOutput", context.buffer->GetInstance<MainBuffer>().GetBuffer(MainBuffer::Layout::SceneOutput).GetDescriptorUAV().GetGPUHandle());

	pipeline.BindShaderParameter(command, parameter);

	const Vector3u& threadGroupSize = pipeline.GetThreadGroupSize();
	commandList->Dispatch(
		Graphics::RoundUp(data.outputResolution.x, threadGroupSize.x),
		Graphics::RoundUp(data.outputResolution.y, threadGroupSize.y),
		1
	);

}

void RenderPassUpscaling::UpscalingNIS(Graphics::GraphicsCommandContext& command, const PassContext& context, const PassContext::ContextData& data) {

	auto commandList     = command.GetCommandList();
	const auto& pipeline = pipelines_[Pipeline::NIS];

	pipeline.BindPipeline(command);

	Graphics::ShaderParameter parameter;

	//!< 共通parameterの設定
	parameter.Set32bitConstants("Context", data);
	parameter.Set32bitConstants("PassData", data_);

	//!< Render情報の設定
	parameter.SetHandle("gSceneRender", context.buffer->GetInstance<MainBuffer>().GetBuffer(MainBuffer::Layout::SceneRender).GetDescriptorSRV().GetGPUHandle());

	//!< Output情報の設定
	parameter.SetHandle("gSceneOutput", context.buffer->GetInstance<MainBuffer>().GetBuffer(MainBuffer::Layout::SceneOutput).GetDescriptorUAV().GetGPUHandle());

	//!< Coefficient情報
	parameter.SetHandle("gCoefScaler", upscalerNIS_.GetCoefficientsScale().GetDescriptor().GetGPUHandle());
	parameter.SetHandle("gCoefUsm",    upscalerNIS_.GetCoefficientsUsm().GetDescriptor().GetGPUHandle());

	pipeline.BindShaderParameter(command, parameter);

	//!< NISは1グループで1ブロックを処理するため, ブロックサイズで割る.
	command.GetCommandList()->Dispatch(
		Graphics::RoundUp(data.outputResolution.x, UpscalerNIS::kBlockWidth),
		Graphics::RoundUp(data.outputResolution.y, UpscalerNIS::kBlockHeight),
		1
	);

}
