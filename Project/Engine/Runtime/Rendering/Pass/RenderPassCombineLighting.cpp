#include "RenderPassCombineLighting.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "../Buffer/MainBuffer.h"
#include "../Buffer/LightingBuffer.h"

//* engine
#include <Runtime/Graphics/Core.h>

//* lib
#include <Lib/Logger/StackLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPassCombineLighting class methods
////////////////////////////////////////////////////////////////////////////////////////////

RenderPassCombineLighting::RenderPassCombineLighting() {

	{ //!< Combine Lighting Compute Pipeline
		Graphics::ComputePipelineState::Desc desc = {};
		desc.SetShaderBlob(Graphics::Core::CompileShader(L"Engine/Packages/shaders/RenderPass/CombineLighting/Combine.cs.hlsl", Graphics::CompileProfile::Compute, L"main"));

		pipelines_[Pipeline::Combine] = Graphics::ReflectedComputePipelineState::Create(
			Graphics::Core::GetDevice(),
			desc
		);
	}

}

void RenderPassCombineLighting::Setup(MAYBE_UNUSED const PassContext& context) {
	//!< bufferの取得
	context.buffer->EnsureInstance<MainBuffer>(context.GetBufferResolution());
}

void RenderPassCombineLighting::Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) {
		
	if (!context.buffer->HasInstance<LightingBuffer>()) {
		STACK_LOG_WARNING("Rendering::RenderPassCombineLighting | <LightingBuffer> is not found.");
		return; //!< Bufferが存在しない場合, 実行しない.
	}

	BaseRenderPass::BeginEvent(command, "Combine Lighting");

	{ //!< Combine Pass
		BaseRenderPass::BeginEvent(command, "Combine Pass");

		BeginCombineLightingPass(command, *context.buffer);

		Combine(command, context, context.GetContextData());

		EndCombineLightingPass(command, *context.buffer);

		BaseRenderPass::EndEvent(command);
	}

	BaseRenderPass::EndEvent(command);

}

void RenderPassCombineLighting::BeginCombineLightingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

	auto commandList = command.GetCommandList();

	//!< bufferの取得
	MainBuffer& main = buffer.GetInstance<MainBuffer>();

	static const size_t kBufferCount = 1;
	ArrayEnum<RenderTargetTexture*, kBufferCount, MainBuffer::Layout> buffers = {
		&main.GetBuffer(MainBuffer::Layout::SceneRender),
	};

	{ //!< barrierの設定

		//!< MainBufferをUnorderedAccessに遷移
		for (EnumUtil<MainBuffer::Layout>::Underlying i = 0; i < kBufferCount; ++i) {
			buffers[i]->Transition(command, RenderTargetTexture::State::UnorderedAccess);
		}
	}

}

void RenderPassCombineLighting::EndCombineLightingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

	auto commandList = command.GetCommandList();

	//!< bufferの取得
	MainBuffer& main = buffer.GetInstance<MainBuffer>();

	static const size_t kBufferCount = 1;
	ArrayEnum<RenderTargetTexture*, kBufferCount, MainBuffer::Layout> buffers = {
		&main.GetBuffer(MainBuffer::Layout::SceneRender),
	};

	{ //!< barrierの設定

		//!< MainBufferを通常状態に遷移
		for (EnumUtil<MainBuffer::Layout>::Underlying i = 0; i < kBufferCount; ++i) {
			buffers[i]->Transition(command, RenderTargetTexture::State::Default);
		}
	}

}

void RenderPassCombineLighting::Combine(Graphics::GraphicsCommandContext& command, const PassContext& context, const PassContext::ContextData& data) {

	auto commandList     = command.GetCommandList();
	const auto& pipeline = pipelines_[Pipeline::Combine];

	pipeline.BindPipeline(command);

	Graphics::ShaderParameter parameter;

	//!< 共通parameterの設定
	parameter.Set32bitConstants("Context", data);
	parameter.Set32bitConstants("PassData", data_);

	//!< Lighting情報の設定
	parameter.SetHandle("gDirect",   context.buffer->GetInstance<LightingBuffer>().GetBuffer(LightingBuffer::Layout::Direct).GetDescriptorSRV().GetGPUHandle());
	parameter.SetHandle("gIndirect", context.buffer->GetInstance<LightingBuffer>().GetBuffer(LightingBuffer::Layout::Indirect).GetDescriptorSRV().GetGPUHandle());

	//!< Scene情報の設定
	parameter.SetHandle("gScene", context.buffer->GetInstance<MainBuffer>().GetBuffer(MainBuffer::Layout::SceneRender).GetDescriptorUAV().GetGPUHandle());

	pipeline.BindShaderParameter(command, parameter);

	const Vector3u& threadGroupSize = pipeline.GetThreadGroupSize();
	commandList->Dispatch(
		Graphics::RoundUp(context.resolution.x, threadGroupSize.x),
		Graphics::RoundUp(context.resolution.y, threadGroupSize.y),
		1
	);
}
