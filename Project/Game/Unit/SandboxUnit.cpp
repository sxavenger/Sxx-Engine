#include "SandboxUnit.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine [platform]
#include <Engine/Runtime/Platform/Input/Input.h>

//* engine [graphics]
#include <Engine/Runtime/Graphics/Core.h>

//* engine [framework]
#include <Engine/Runtime/Framework/Core/Context.h>

//* engine [assets]
#include <Engine/Runtime/Assets/Texture/Texture.h>
#include <Engine/Runtime/Assets/Mesh/StaticMesh.h>
#include <Engine/Runtime/Assets/Handle/AssetHandle.h>

//* world [world]
#include <Engine/Runtime/World/Component/Transform/TransformComponent.h>
#include <Engine/Runtime/World/Component/Camera/CameraComponent.h>

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

	{ //!< Graphics Pipeline State 0 の作成
		Sxx::Graphics::GraphicsPipelineState::Desc desc = {};
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/Sample/Simple.vs.hlsl", Sxx::Graphics::CompileProfile::Vertex, L"main"));
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/Sample/Simple.ps.hlsl", Sxx::Graphics::CompileProfile::Pixel, L"main"));
		desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
		desc.SetDepthStencil(true);
		desc.SetDepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT);
		desc.SetBlendMode(0, Sxx::Graphics::BlendModeColor::None, Sxx::Graphics::BlendModeTransparent::None);
		desc.SetPrimitive(Sxx::Graphics::PrimitiveTopology::TriangleList);
		desc.AppendRenderTargetFormat(DXGI_FORMAT_R16G16B16A16_FLOAT);

		pipeline0_ = Sxx::Graphics::ReflectedGraphicsPipelineState::Create(
			Sxx::Graphics::Core::GetDevice(),
			desc
		);
	}

	{ //!< Graphics Pipeline State 1 の作成
		Sxx::Graphics::GraphicsPipelineState::Desc desc = {};
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/Present/Present.vs.hlsl", Sxx::Graphics::CompileProfile::Vertex, L"main"));
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/Present/Present.ps.hlsl", Sxx::Graphics::CompileProfile::Pixel, L"main"));
		desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID, false);
		desc.SetDepthStencil(false);
		desc.SetBlendMode(0, Sxx::Graphics::BlendModeColor::None, Sxx::Graphics::BlendModeTransparent::None);
		desc.SetPrimitive(Sxx::Graphics::PrimitiveTopology::TriangleList);
		desc.AppendRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

		pipeline1_ = Sxx::Graphics::ReflectedGraphicsPipelineState::Create(
			Sxx::Graphics::Core::GetDevice(),
			desc
		);
	}

	{ //!< Mesh Asset の読み込みテスト

		Sxx::Assets::AssetHandle<Sxx::Assets::StaticMesh> handle;
#ifdef _DEBUG
		//!< DEBUGの場合はcube読み込み
		handle = Sxx::Assets::AssetStorage::GetInstance()->Import<Sxx::Assets::StaticMesh>(L"Assets/common/StaticMeshes/cube.asset");
#else
		//!< DEVELOP, RELEASEの場合はbunny読み込み
		handle = Sxx::Assets::AssetStorage::GetInstance()->Import<Sxx::Assets::StaticMesh>(L"Assets/bunny/StaticMeshes/stanford_bunny.asset");
#endif

		auto mesh = handle.WaitGet();

		cache_.Cache(mesh);
	}

	{ //!< Render Target Textureの作成
		Sxx::Rendering::RenderTargetTexture::Options options = {};
		options.format     = DXGI_FORMAT_R16G16B16A16_FLOAT;
		options.resolution = { 1280, 720 };

		renderTarget_ = Sxx::Rendering::RenderTargetTexture::Create(options);
	}

	{ //!< Depth Stencil Textureの作成
		Sxx::Rendering::DepthStencilTexture::Options options = {};
		options.format     = DXGI_FORMAT_D24_UNORM_S8_UINT;
		options.resolution = { 1280, 720 };

		depthStencil_ = Sxx::Rendering::DepthStencilTexture::Create(options);
	}

	{
		object_->AddComponent<Sxx::World::CameraComponent>();
		object_->AddComponent<Sxx::World::TransformComponent>();

		Sxx::World::CameraComponent::Perspective perspective = {};
		perspective.sensor   = { 16.0f, 9.0f };
		perspective.focal    = 20.0f;
		perspective.nearClip = 0.1f;
		perspective.farClip  = 1024.0f;

		auto camera = object_->GetComponent<Sxx::World::CameraComponent>();
		camera->SetProjection(perspective);

		auto transform = object_->GetComponent<Sxx::World::TransformComponent>();
		transform->SetPosition({ 0.0f, 1.0f, -20.0f });
	}
}

void SandboxUnit::TermSandbox() {
}

void SandboxUnit::UpdateSandbox() {

	{
		auto camera = object_->GetComponent<Sxx::World::CameraComponent>();
		cameraCache_.Cache(*camera);
	}

	{
		const auto& keyboard = Sxx::Platform::Input::GetKeyboard();

		auto transform = object_->GetComponent<Sxx::World::TransformComponent>();

		Vector3f position = transform->GetPosition();

		if (keyboard.GetKey(Sxx::Platform::KeyId::A).IsPress()) {
			position.x -= 0.1f;
		}

		if (keyboard.GetKey(Sxx::Platform::KeyId::D).IsPress()) {
			position.x += 0.1f;
		}

		if (keyboard.GetKey(Sxx::Platform::KeyId::W).IsPress()) {
			position.z += 0.1f;
		}

		if (keyboard.GetKey(Sxx::Platform::KeyId::S).IsPress()) {
			position.z -= 0.1f;
		}

		if (keyboard.GetKey(Sxx::Platform::KeyId::Q).IsPress()) {
			position.y += 0.1f;
		}

		if (keyboard.GetKey(Sxx::Platform::KeyId::E).IsPress()) {
			position.y -= 0.1f;
		}

		transform->SetPosition(position);

		transform->Update();

		transformCache_.Cache(*transform);
	}
}

void SandboxUnit::RenderSandbox() {

	auto& context = Sxx::Graphics::Core::GetCommandContextDirect();

	{ //!< RenderTargetへの書き込み

		auto commandList = context.GetCommandList();

		renderTarget_.Transition(context, D3D12_RESOURCE_STATE_RENDER_TARGET);
		renderTarget_.ClearRenderTarget(context);

		depthStencil_.Transition(context, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		depthStencil_.ClearDepthStencil(context);

		commandList->OMSetRenderTargets(
			1, &renderTarget_.GetDescriptorRTV().GetCPUHandle(), false,
			&depthStencil_.GetDescriptorDSV().GetCPUHandle()
		);

		D3D12_INDEX_BUFFER_VIEW ibv = cache_.GetIndexBuffer().GetIndexBufferView();
		commandList->IASetIndexBuffer(&ibv);

		pipeline0_.BindPipeline(context, { 1280, 720 });

		Sxx::Graphics::ShaderParameter parameter;
		parameter.SetAddress("gPositions", cache_.GetPositionVertexBuffer().positions.GetGpuVirtualAddress());
		parameter.SetAddress("gVertices", cache_.GetStaticMeshVertexBuffer().vertices.GetGpuVirtualAddress());
		parameter.SetAddress("gCameraProjection", cameraCache_.GetProjectionBuffer().GetGpuVirtualAddress());
		parameter.SetAddress("gCameraTransform",  transformCache_.GetTransformationBuffer().GetGpuVirtualAddress());

		pipeline0_.BindShaderParameter(context, parameter);

		commandList->DrawIndexedInstanced(cache_.GetIndexBuffer().GetIndexCount(), 1, 0, 0, 0);

		renderTarget_.Transition(context, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		depthStencil_.Transition(context, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}

	if (Sxx::Framework::Context::HasUnit<Sxx::WindowUnit>()) {

		RefPtr<Sxx::WindowUnit> unit
			= Sxx::Framework::Context::GetUnit<Sxx::WindowUnit>(); //!< windowを管理しているunitを取得.

		auto& viewport = unit->GetViewport();

		viewport.BeginRenderPass(context, Color4f::Convert(0x9BA8A8FF));

		{ //!< Shaderでの書き込み

			auto commandList = context.GetCommandList();

			pipeline1_.BindPipeline(context, viewport.GetClient());

			Sxx::Graphics::ShaderParameter parameter;
			parameter.SetHandle("gTexture", renderTarget_.GetDescriptorSRV().GetGPUHandle());

			pipeline1_.BindShaderParameter(context, parameter);

			commandList->DrawInstanced(3, 1, 0, 0);
		}

		viewport.EndRenderPass(context);
	}
}
