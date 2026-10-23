#include "RenderPassDirectLighting.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "../Buffer/LightingBuffer.h"
#include "../Buffer/GBuffer.h"
#include "../Buffer/DepthStencilBuffer.h"

//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPassDirectLighting class methods
////////////////////////////////////////////////////////////////////////////////////////////

RenderPassDirectLighting::RenderPassDirectLighting() {

	{ //!< Forward Descの作成

		Sxx::Graphics::GraphicsPipelineState::Desc& desc = descs_[Desc::Forward];
		desc.SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
		desc.SetBlendMode(0, Sxx::Graphics::BlendModeColor::Additive, Sxx::Graphics::BlendModeTransparent::Max);
		desc.SetPrimitive(Sxx::Graphics::PrimitiveTopology::TriangleList);
		desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_GREATER);
		desc.SetDepthStencilFormat(DepthStencilBuffer::GetFormat(DepthStencilBuffer::Layout::Scene));
		desc.AppendRenderTargetFormat(LightingBuffer::GetFormat(LightingBuffer::Layout::Direct));
	}

	{ //!< Directional Light Pipelineの作成

		Sxx::Graphics::GraphicsPipelineState::Desc desc = descs_[Desc::Forward];
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/RenderPass/DirectLighting/Fullscreen.vs.hlsl", Sxx::Graphics::CompileProfile::Vertex, L"main"));
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/RenderPass/DirectLighting/DirectionalLight.ps.hlsl", Sxx::Graphics::CompileProfile::Pixel, L"main"));

		pipelines_[Pipeline::DirectionalLight] = Graphics::ReflectedGraphicsPipelineState::Create(
			Graphics::Core::GetDevice(),
			desc
		);
	}

}

void RenderPassDirectLighting::Setup(const PassContext& context) {
	//!< bufferの確保
	context.buffer->EnsureInstance<LightingBuffer>(context.resolution);
	context.buffer->EnsureInstance<GBuffer>(context.resolution);
	context.buffer->EnsureInstance<DepthStencilBuffer>(context.resolution);
}

void RenderPassDirectLighting::Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) {

	FlagEnum<PassContext::Issue> issues = PassContext::CheckIssue(context);

	if (issues.Test(PassContext::Issue::Warning_Scene) || issues.Test(PassContext::Issue::Warning_Camera)) {
		return; //!< PassContextに警告がある場合, 実行を中止する
	}

	BaseRenderPass::BeginEvent(command, "Direct Lighting");

	{ //!< Direct Lighting Pass

		BeginDirectLightingPass(command, *context.buffer);

		LightingDirectionalLight(command, context, context.GetContextData());

		EndDirectLightingPass(command, *context.buffer);
	}

	BaseRenderPass::EndEvent(command);

}

void RenderPassDirectLighting::BeginDirectLightingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

	auto commandList = command.GetCommandList();

	//!< bufferの取得
	LightingBuffer& lighting         = buffer.GetInstance<LightingBuffer>();
	DepthStencilBuffer& depthStencil = buffer.GetInstance<DepthStencilBuffer>();

	static const size_t kBufferCount = 1;
	ArrayEnum<RenderTargetTexture*, kBufferCount, LightingBuffer::Layout> buffers = {
		&lighting.GetBuffer(LightingBuffer::Layout::Direct)
	};

	{ //!< barrierの設定

		//!< LightingBufferをRenderTargetに遷移
		for (EnumUtil<LightingBuffer::Layout>::Underlying i = 0; i < kBufferCount; ++i) {
			buffers[i]->Transition(command, RenderTargetTexture::State::RenderTarget);
		}

		//!< DepthStencilをDepthWriteに遷移
		depthStencil.GetBuffer(DepthStencilBuffer::Layout::Scene).Transition(command, DepthStencilTexture::State::Read);
	}

	{ //!< Render Targetの設定

		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kBufferCount> handles = {};
		for (size_t i = 0; i < kBufferCount; ++i) {
			handles[i] = buffers[i]->GetDescriptorRTV().GetCPUHandle();
		}

		commandList->OMSetRenderTargets(
			static_cast<UINT>(handles.size()), handles.data(), false,
			&depthStencil.GetBuffer(DepthStencilBuffer::Layout::Scene).GetDescriptorDSV().GetCPUHandle()
		);

	}

}

void RenderPassDirectLighting::EndDirectLightingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

	auto commandList = command.GetCommandList();

	//!< bufferの取得
	LightingBuffer& lighting = buffer.GetInstance<LightingBuffer>();
	DepthStencilBuffer& depthStencil = buffer.GetInstance<DepthStencilBuffer>();

	static const size_t kBufferCount = 1;
	ArrayEnum<RenderTargetTexture*, kBufferCount, LightingBuffer::Layout> buffers = {
		&lighting.GetBuffer(LightingBuffer::Layout::Direct)
	};

	{ //!< barrierの設定

		//!< LightingBufferを通常状態に遷移
		for (EnumUtil<LightingBuffer::Layout>::Underlying i = 0; i < kBufferCount; ++i) {
			buffers[i]->Transition(command, RenderTargetTexture::State::Default);
		}

		//!< DepthStencilを通常状態に遷移
		depthStencil.GetBuffer(DepthStencilBuffer::Layout::Scene).Transition(command, DepthStencilTexture::State::Default);
	}

}

void RenderPassDirectLighting::LightingDirectionalLight(Graphics::GraphicsCommandContext& command, const PassContext& context, const PassContext::ContextData& data) {

	auto commandList = command.GetCommandList();
	const auto& pipeline = pipelines_[Pipeline::DirectionalLight];

	//!< bufferの取得
	const GBuffer& gbuffer                 = context.buffer->GetInstance<GBuffer>();
	const DepthStencilBuffer& depthStencil = context.buffer->GetInstance<DepthStencilBuffer>();

	pipeline.BindPipeline(command, context.resolution);

	Graphics::ShaderParameter parameter;

	//!< 共通parameterの設定
	parameter.Set32bitConstants("Context", data);
	parameter.Set32bitConstants("PassData", data_);

	//!< camera情報の設定
	parameter.SetAddress("gCameraProjection", context.camera.camera->GetProjectionBuffer().GetGpuVirtualAddress());
	parameter.SetAddress("gCameraTransform",  context.camera.transform->GetTransformationBuffer().GetGpuVirtualAddress());

	//!< scene情報の設定
	parameter.SetAddress("gScene", context.scene->GetTopLevelAccelerationStructure().GetGpuVirtualAddress());

	//!< GBuffer情報の設定
	parameter.SetHandle("gDepth",   depthStencil.GetBuffer(DepthStencilBuffer::Layout::Scene).GetDescriptorSRV().GetGPUHandle());
	parameter.SetHandle("gBufferA", gbuffer.GetBuffer(GBuffer::Layout::GBufferA).GetDescriptorSRV().GetGPUHandle());
	parameter.SetHandle("gBufferB", gbuffer.GetBuffer(GBuffer::Layout::GBufferB).GetDescriptorSRV().GetGPUHandle());
	parameter.SetHandle("gBufferC", gbuffer.GetBuffer(GBuffer::Layout::GBufferC).GetDescriptorSRV().GetGPUHandle());

	for (const auto& instance : context.scene->directionalLights) {

		//!< light情報の設定
		parameter.SetAddress("gDirectionalLight", instance.light->GetDirectionalLightBuffer().GetGpuVirtualAddress());
		parameter.SetAddress("gLightTransform",   instance.transform->GetTransformationBuffer().GetGpuVirtualAddress());

		pipeline.BindShaderParameter(command, parameter);

		commandList->DrawInstanced(3, 1, 0, 0); //!< フルスクリーンパスの描画
		
	}
}
