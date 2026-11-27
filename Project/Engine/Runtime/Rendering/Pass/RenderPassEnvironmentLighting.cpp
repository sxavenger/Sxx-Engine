#include "RenderPassEnvironmentLighting.h"
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
// RenderPassEnvironmentLighting class methods
////////////////////////////////////////////////////////////////////////////////////////////

RenderPassEnvironmentLighting::RenderPassEnvironmentLighting() {

	Graphics::BlendState::BlendDesc colorDesc = Graphics::BlendState::BlendDesc{ true, D3D12_BLEND_ONE, D3D12_BLEND_ONE, D3D12_BLEND_OP_ADD }; //!< 加算合成の設定

	{ //!< Forward Descの作成

		Graphics::GraphicsPipelineState::Desc& desc = descs_[Desc::Forward];
		desc.SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
		desc.SetBlendMode(0, colorDesc, Graphics::BlendModeTransparent::Max);
		desc.SetPrimitive(Graphics::PrimitiveTopology::TriangleList);
		desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_GREATER);
		desc.SetDepthStencilFormat(DepthStencilBuffer::GetFormat(DepthStencilBuffer::Layout::SceneRender));
		desc.AppendRenderTargetFormat(LightingBuffer::GetFormat(LightingBuffer::Layout::Direct));
	}

	{ //!< Back Descの作成

		Graphics::GraphicsPipelineState::Desc& desc = descs_[Desc::Back];
		desc.SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
		desc.SetBlendMode(0, colorDesc, Graphics::BlendModeTransparent::Max);
		desc.SetPrimitive(Graphics::PrimitiveTopology::TriangleList);
		desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_EQUAL);
		desc.SetDepthStencilFormat(DepthStencilBuffer::GetFormat(DepthStencilBuffer::Layout::SceneRender));
		desc.AppendRenderTargetFormat(LightingBuffer::GetFormat(LightingBuffer::Layout::Direct));
		
	}

	{ //!< Background Pipelineの作成

		Graphics::GraphicsPipelineState::Desc desc = descs_[Desc::Back];
		desc.SetShaderBlob(Graphics::Core::CompileShader(L"Engine/Packages/shaders/RenderPass/EnvironmentLighting/Fullscreen.vs.hlsl", Graphics::CompileProfile::Vertex, L"main"));
		desc.SetShaderBlob(Graphics::Core::CompileShader(L"Engine/Packages/shaders/RenderPass/EnvironmentLighting/Background.ps.hlsl", Graphics::CompileProfile::Pixel, L"main"));

		pipelines_[Pipeline::Background] = Graphics::ReflectedGraphicsPipelineState::Create(
			Graphics::Core::GetDevice(),
			desc
		);
	}
}

void RenderPassEnvironmentLighting::Setup(MAYBE_UNUSED const PassContext& context) {
	//!< bufferの確保
	context.buffer->EnsureInstance<LightingBuffer>(context.GetBufferResolution());
	context.buffer->EnsureInstance<GBuffer>(context.GetBufferResolution());
	context.buffer->EnsureInstance<DepthStencilBuffer>(context.GetBufferResolution());
}

void RenderPassEnvironmentLighting::Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) {

	FlagEnum<PassContext::Issue> issues = PassContext::CheckIssue(context);

	if (issues.Test(PassContext::Issue::Warning_Scene) || issues.Test(PassContext::Issue::Warning_Camera)) {
		return; //!< PassContextに警告がある場合, 実行を中止する
	}

	BaseRenderPass::BeginEvent(command, "Environment Lighting");

	{ //!< Environment Lighting Pass

		BeginEnvironmentLightingPass(command, *context.buffer);

		LightingSkyLight(command, context, context.GetContextData());

		EndEnvironmentLightingPass(command, *context.buffer);
	}

	BaseRenderPass::EndEvent(command);

}

void RenderPassEnvironmentLighting::BeginEnvironmentLightingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

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
		depthStencil.GetBuffer(DepthStencilBuffer::Layout::SceneRender).Transition(command, DepthStencilTexture::State::Read);
	}

	{ //!< Render Targetの設定

		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kBufferCount> handles = {};
		for (size_t i = 0; i < kBufferCount; ++i) {
			handles[i] = buffers[i]->GetDescriptorRTV().GetCPUHandle();
		}

		commandList->OMSetRenderTargets(
			static_cast<UINT>(handles.size()), handles.data(), false,
			&depthStencil.GetBuffer(DepthStencilBuffer::Layout::SceneRender).GetDescriptorDSV().GetCPUHandle()
		);

	}

}

void RenderPassEnvironmentLighting::EndEnvironmentLightingPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

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
		depthStencil.GetBuffer(DepthStencilBuffer::Layout::SceneRender).Transition(command, DepthStencilTexture::State::Default);
	}

}

void RenderPassEnvironmentLighting::LightingSkyLight(Graphics::GraphicsCommandContext& command, const PassContext& context, const PassContext::ContextData& data) {

	auto commandList = command.GetCommandList();

	//!< bufferの取得
	const GBuffer& gbuffer                 = context.buffer->GetInstance<GBuffer>();
	const DepthStencilBuffer& depthStencil = context.buffer->GetInstance<DepthStencilBuffer>();

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
	parameter.SetHandle("gDepth",   depthStencil.GetBuffer(DepthStencilBuffer::Layout::SceneRender).GetDescriptorSRV().GetGPUHandle());
	parameter.SetHandle("gBufferA", gbuffer.GetBuffer(GBuffer::Layout::GBufferA).GetDescriptorSRV().GetGPUHandle());
	parameter.SetHandle("gBufferB", gbuffer.GetBuffer(GBuffer::Layout::GBufferB).GetDescriptorSRV().GetGPUHandle());
	parameter.SetHandle("gBufferC", gbuffer.GetBuffer(GBuffer::Layout::GBufferC).GetDescriptorSRV().GetGPUHandle());

	{ //!< 背景の描画

		const auto& pipeline = pipelines_[Pipeline::Background];
		pipeline.BindPipeline(command, data.renderResolution);

		Graphics::ShaderParameter p = parameter;

		for (const auto& instance : context.scene->skyLights) {

			//!< light情報の設定
			p.SetAddress("gSkyLight", instance.light->GetLightBuffer().GetGpuVirtualAddress());

			pipeline.BindShaderParameter(command, p);

			commandList->DrawInstanced(3, 1, 0, 0); //!< フルスクリーンパスの描画
		}

	}

}
