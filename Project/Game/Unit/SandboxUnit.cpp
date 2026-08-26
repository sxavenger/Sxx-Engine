#include "SandboxUnit.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine [graphics]
#include <Engine/Runtime/Graphics/Core.h>

//* engine [framework]
#include <Engine/Runtime/Framework/Core/Context.h>

//* engine [assets]
#include <Engine/Runtime/Assets/Texture/Texture.h>
#include <Engine/Runtime/Assets/Mesh/StaticMesh.h>
#include <Engine/Runtime/Assets/Handle/AssetHandle.h>

//* engine [rendering]
#include <Engine/Runtime/Rendering/Cache/StaticMeshCache.h>

//* engine [unit]
#include <Engine/Unit/WindowUnit.h>
#include <Engine/Unit/SlateEditorUnit.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SandboxUnit class methods
////////////////////////////////////////////////////////////////////////////////////////////

SandboxUnit::SandboxUnit() {
#ifdef DEVELOPMENT
	//Sxx::Framework::Context::Push<Sxx::SlateEditorUnit>();
	Sxx::Framework::Context::Push<Sxx::WindowUnit>();
#else
	//!< windowの追加
	Sxx::Framework::Context::Push<Sxx::WindowUnit>();
#endif
}

SandboxUnit::~SandboxUnit() {
}

void SandboxUnit::Setup(Sxx::Framework::Pipeline& pipeline) {
	pipeline.SetProcess(Sxx::Framework::Phase::Initialize, Sxx::Framework::Priority::Normal, [this]() { InitSandbox(); });
	pipeline.SetProcess(Sxx::Framework::Phase::Terminate, Sxx::Framework::Priority::Normal, [this]() { TermSandbox(); });
	pipeline.SetProcess(Sxx::Framework::Phase::Update, Sxx::Framework::Priority::Normal, [this]() { UpdateSandbox(); });
	pipeline.SetProcess(Sxx::Framework::Phase::Render, Sxx::Framework::Priority::Normal, [this]() { RenderSandbox(); });
}

void SandboxUnit::InitSandbox() {

	{ //!< Graphics Pipeline Stateの作成
		Sxx::Graphics::GraphicsPipelineState::Desc desc = {};
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/Test.vs.hlsl", Sxx::Graphics::CompileProfile::Vertex, L"main"));
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/Test.ps.hlsl", Sxx::Graphics::CompileProfile::Pixel, L"main"));
		desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID, false);
		desc.SetDepthStencil(false);
		desc.SetBlendMode(0, Sxx::Graphics::BlendModeColor::None, Sxx::Graphics::BlendModeTransparent::None);
		desc.SetPrimitive(Sxx::Graphics::PrimitiveTopology::TriangleList);
		desc.AppendRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

		pipeline_ = Sxx::Graphics::ReflectedGraphicsPipelineState::Create(
			Sxx::Graphics::Core::GetDevice(),
			desc
		);
	}

	{ //!< Constant Buffer の作成
		buffer_ = Sxx::Graphics::Core::CreateConstantBuffer<Color4f>(Sxx::Graphics::kFrameCount);
		buffer_.SetName(L"Color Buffer");
	}

	{ //!< Mesh Asset の読み込みテスト
		Sxx::Assets::AssetHandle<Sxx::Assets::StaticMesh> handle =
			Sxx::Assets::AssetStorage::GetInstance()->Import<Sxx::Assets::StaticMesh>(L"Assets/common/StaticMeshes/cube.asset");
		auto mesh = handle.WaitGet();

		Sxx::Rendering::StaticMeshCache cache;
		cache.Cache(mesh);
	}
}

void SandboxUnit::TermSandbox() {
}

void SandboxUnit::UpdateSandbox() {
	buffer_.At() = Color4f::Blue();
}

void SandboxUnit::RenderSandbox() {

	auto& context = Sxx::Graphics::Core::GetCommandContextDirect();

	if (Sxx::Framework::Context::HasUnit<Sxx::WindowUnit>()) {

		RefPtr<Sxx::WindowUnit> unit
			= Sxx::Framework::Context::GetUnit<Sxx::WindowUnit>(); //!< windowを管理しているunitを取得.

		auto& viewport = unit->GetViewport();

		viewport.BeginRenderPass(context, Color4f::Convert(0x9BA8A8FF));

		{ //!< Shaderでの書き込み
			auto commandList = context.GetCommandList();

			pipeline_.BindPipeline(context, viewport.GetClient());

			Sxx::Graphics::ShaderParameter parameter;
			parameter.SetAddress("gColor", buffer_.GetGpuVirtualAddress());

			pipeline_.BindShaderParameter(context, parameter);
			commandList->DrawInstanced(3, 1, 0, 0);
		}

		viewport.EndRenderPass(context);
	}
}
