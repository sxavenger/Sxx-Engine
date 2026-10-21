#include "RenderPassDeferredBase.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "../Buffer/GBuffer.h"
#include "../Buffer/DepthStencilBuffer.h"

//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPassDeferredBase class
////////////////////////////////////////////////////////////////////////////////////////////

RenderPassDeferredBase::RenderPassDeferredBase() {

	{ //!< Opaque Mesh Render Pipeline

		Sxx::Graphics::GraphicsPipelineState::Desc desc = {};
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/RenderPass/DeferredBase/Deferred.as.hlsl", Sxx::Graphics::CompileProfile::Amplification, L"main"));
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/RenderPass/DeferredBase/Deferred.ms.hlsl", Sxx::Graphics::CompileProfile::Mesh, L"main"));
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/RenderPass/DeferredBase/Deferred.ps.hlsl", Sxx::Graphics::CompileProfile::Pixel, L"main"));
		desc.SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
		desc.SetBlendMode(0, Sxx::Graphics::BlendModeColor::None, Sxx::Graphics::BlendModeTransparent::None);
		desc.SetPrimitive(Sxx::Graphics::PrimitiveTopology::TriangleList);
		desc.SetDepthStencil(true);
		desc.SetDepthStencilFormat(DepthStencilBuffer::GetFormat(DepthStencilBuffer::Layout::Scene));
		desc.AppendRenderTargetFormat(GBuffer::GetFormat(GBuffer::Layout::GBufferA));
		desc.AppendRenderTargetFormat(GBuffer::GetFormat(GBuffer::Layout::GBufferB));
		desc.AppendRenderTargetFormat(GBuffer::GetFormat(GBuffer::Layout::GBufferC));

		pipeline_ = Graphics::ReflectedGraphicsPipelineState::Create(
			Graphics::Core::GetDevice(),
			desc
		);
	}


}

void RenderPassDeferredBase::Setup(const PassContext& context) {
	//!< bufferの確保
	context.buffer->EnsureInstance<GBuffer>(context.resolution);
	context.buffer->EnsureInstance<DepthStencilBuffer>(context.resolution);
}

void RenderPassDeferredBase::Execute(Graphics::GraphicsCommandContext& command, const PassContext& context) {

	FlagEnum<PassContext::Issue> issues = PassContext::CheckIssue(context);

	if (issues.Test(PassContext::Issue::Warning_Scene) || issues.Test(PassContext::Issue::Warning_Camera)) {
		return; //!< PassContextに警告がある場合, 実行を中止する
	}

	BaseRenderPass::BeginEvent(command, "Deferred Base");

	{ //!< Opaque Mesh Render Pass
		BaseRenderPass::BeginEvent(command, "Opaque Mesh Render Pass");

		BeginOpaqueMeshRenderPass(command, *context.buffer);

		RenderStaticMesh(command, context);

		EndOpaqueMeshRenderPass(command, *context.buffer);

		BaseRenderPass::EndEvent(command);
	}

	BaseRenderPass::EndEvent(command);
}

void RenderPassDeferredBase::BeginOpaqueMeshRenderPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

	auto commandList = command.GetCommandList();

	//!< bufferの取得
	GBuffer& gbuffer                 = buffer.GetInstance<GBuffer>();
	DepthStencilBuffer& depthStencil = buffer.GetInstance<DepthStencilBuffer>();

	static const size_t kBufferCount = EnumUtil<GBuffer::Layout>::GetCount();
	ArrayEnum<RenderTargetTexture*, kBufferCount, GBuffer::Layout> buffers = {
		&gbuffer.GetBuffer(GBuffer::Layout::GBufferA),
		&gbuffer.GetBuffer(GBuffer::Layout::GBufferB),
		&gbuffer.GetBuffer(GBuffer::Layout::GBufferC)
	};

	{ //!< barrierの設定

		//!< GBufferをRenderTargetに遷移
		for (EnumUtil<GBuffer::Layout>::Underlying i = 0; i < kBufferCount; ++i) {
			buffers[i]->Transition(command, RenderTargetTexture::State::RenderTarget);
		}

		//!< DepthStencilをDepthWriteに遷移
		depthStencil.GetBuffer(DepthStencilBuffer::Layout::Scene).Transition(command, DepthStencilTexture::State::Write);
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

void RenderPassDeferredBase::EndOpaqueMeshRenderPass(Graphics::GraphicsCommandContext& command, const BufferCollection& buffer) {

	auto commandList = command.GetCommandList();

	//!< bufferの取得
	GBuffer& gbuffer                 = buffer.GetInstance<GBuffer>();
	DepthStencilBuffer& depthStencil = buffer.GetInstance<DepthStencilBuffer>();

	static const size_t kBufferCount = EnumUtil<GBuffer::Layout>::GetCount();
	ArrayEnum<RenderTargetTexture*, kBufferCount, GBuffer::Layout> buffers = {
		&gbuffer.GetBuffer(GBuffer::Layout::GBufferA),
		&gbuffer.GetBuffer(GBuffer::Layout::GBufferB),
		&gbuffer.GetBuffer(GBuffer::Layout::GBufferC)
	};

	{ //!< barrierの設定

		//!< GBufferを通常状態に遷移
		for (EnumUtil<GBuffer::Layout>::Underlying i = 0; i < kBufferCount; ++i) {
			buffers[i]->Transition(command, RenderTargetTexture::State::Default);
		}

		//!< DepthStencilを通常状態に遷移
		depthStencil.GetBuffer(DepthStencilBuffer::Layout::Scene).Transition(command, DepthStencilTexture::State::Default);
	}

}

void RenderPassDeferredBase::RenderStaticMesh(Graphics::GraphicsCommandContext& command, const PassContext& context) {

	auto commandList = command.GetCommandList();

	pipeline_.BindPipeline(command, context.resolution);

	Graphics::ShaderParameter parameter;

	//!< 共通parameterの設定
	parameter.Set32bitConstants("Context", context.resolution);
	parameter.Set32bitConstants("PassData", data_);

	//!< camera情報の設定
	parameter.SetAddress("gCameraProjection", context.camera.camera->GetProjectionBuffer().GetGpuVirtualAddress());
	parameter.SetAddress("gCameraTransform",  context.camera.transform->GetTransformationBuffer().GetGpuVirtualAddress());

	//!< static meshの描画
	for (const auto& instance : context.scene->staticMeshes) {

		if (!instance.mesh->HasCache() || !instance.material->HasCache()) {
			continue; //!< キャッシュがない場合は描画しない
		}

		//!< 頂点情報の設定
		parameter.SetAddress("gPositions", instance.mesh->GetPositionVertexBuffer().positions.GetGpuVirtualAddress());
		parameter.SetAddress("gVertices",  instance.mesh->GetStaticMeshVertexBuffer().vertices.GetGpuVirtualAddress());

		//!< メッシュレット情報の設定
		parameter.Set32bitConstants("MeshletData", instance.mesh->GetMeshletBuffer().meshletCount);
		parameter.SetAddress("gMeshlets",      instance.mesh->GetMeshletBuffer().meshlets.GetGpuVirtualAddress());
		parameter.SetAddress("gVertexIndices", instance.mesh->GetMeshletBuffer().vertexIndices.GetGpuVirtualAddress());
		parameter.SetAddress("gTriangles",     instance.mesh->GetMeshletBuffer().triangles.GetGpuVirtualAddress());

		//!< object情報の設定
		parameter.SetAddress("gTransform", instance.transform->GetTransformationBuffer().GetGpuVirtualAddress());
		parameter.SetAddress("gMaterial",  instance.material->GetMaterialBuffer().material.GetGpuVirtualAddress());

		pipeline_.BindShaderParameter(command, parameter);

		commandList->DispatchMesh(Sxx::Graphics::RoundUp(instance.mesh->GetMeshletBuffer().meshletCount, Meshlet::kAmplificationThreadCount), 1, 1);
	}
}
