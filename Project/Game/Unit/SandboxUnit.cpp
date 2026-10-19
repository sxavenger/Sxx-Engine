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
#include <Engine/Runtime/Assets/Importer/TextureImporter.h>
#include <Engine/Runtime/Assets/Importer/SurfaceMaterialImporter.h>
#include <Engine/Runtime/Assets/Importer/StaticMeshImporter.h>

//* world [world]
#include <Engine/Runtime/World/Component/Transform/TransformComponent.h>
#include <Engine/Runtime/World/Component/Camera/CameraComponent.h>
#include <Engine/Runtime/World/Component/Renderer/StaticMeshRenderer.h>

//* engine [rendering]
#include <Engine/Runtime/Rendering/Cache/Static/StaticMeshCache.h>
#include <Engine/Runtime/Rendering/Cache/Static/SurfaceMaterialCache.h>
#include <Engine/Runtime/Rendering/Cache/Dynamic/CameraCache.h>
#include <Engine/Runtime/Rendering/Cache/Dynamic/TransformCache.h>
#include <Engine/Runtime/Rendering/Cache/Manager/CacheCollection.h>

//* engine [unit]
#include <Engine/Unit/WindowUnit.h>
#include <Engine/Unit/SlateEditorUnit.h>

//* lib
#include <Lib/Math/VectorComparison.h>

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
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/Sample/MS-PS/Simple.as.hlsl", Sxx::Graphics::CompileProfile::Amplification, L"main"));
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/Sample/MS-PS/Simple.ms.hlsl", Sxx::Graphics::CompileProfile::Mesh, L"main"));
		desc.SetShaderBlob(Sxx::Graphics::Core::CompileShader(L"Engine/Packages/shaders/Sample/MS-PS/Simple.ps.hlsl", Sxx::Graphics::CompileProfile::Pixel, L"main"));
		desc.SetRasterizer(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
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

	{ //!< Cameraの作成
		camera_->AddComponent<Sxx::World::CameraComponent>();
		camera_->AddComponent<Sxx::World::TransformComponent>();

		Sxx::World::CameraComponent::Perspective perspective = {};
		perspective.sensor   = { 16.0f, 9.0f };
		perspective.focal    = 20.0f;
		perspective.nearClip = 0.1f;
		perspective.farClip  = 1024.0f;

		auto camera = camera_->GetComponent<Sxx::World::CameraComponent>();
		camera->SetProjection(perspective);

		auto transform = camera_->GetComponent<Sxx::World::TransformComponent>();
		transform->SetPosition({ 0.0f, 1.0f, -20.0f });
	}

	{ //!< Objectの作成

		object_->AddComponent<Sxx::World::TransformComponent>();
		object_->AddComponent<Sxx::World::StaticMeshRenderer>();

		auto transform = object_->GetComponent<Sxx::World::TransformComponent>();
		transform->SetPosition({ 0.0f, 0.0f, 0.0f });

		auto renderer = object_->GetComponent<Sxx::World::StaticMeshRenderer>();
		renderer->SetMesh(Sxx::Assets::StaticMeshImporter::Import(L"Assets/DamagedHelmet/DamagedHelmet.gltf", { .index = 0 }));
		renderer->SetMaterial(Sxx::Assets::SurfaceMaterialImporter::Import(L"Assets/DamagedHelmet/DamagedHelmet.gltf", { .index = 0 }));
	}
	
}

void SandboxUnit::TermSandbox() {
}

void SandboxUnit::UpdateSandbox() {

	if (Sxx::Framework::Context::HasUnit<Sxx::WindowUnit>()) {

		RefPtr<Sxx::WindowUnit> unit
			= Sxx::Framework::Context::GetUnit<Sxx::WindowUnit>(); //!< windowを管理しているunitを取得.

		auto& viewport = unit->GetViewport();
		const Vector2u& client = viewport.GetClient();

		{ //!< RenderTargetのリサイズ
			if (Comparison::Any(renderTarget_.GetResolution() != client)) {
				Sxx::Rendering::RenderTargetTexture::Options options = renderTarget_.GetOptions();
				options.resolution = client;

				renderTarget_ = Sxx::Rendering::RenderTargetTexture::Create(options);
			}
		}

		{ //!< DepthStencilのリサイズ
			if (Comparison::Any(depthStencil_.GetResolution() != client)) {
				Sxx::Rendering::DepthStencilTexture::Options options = depthStencil_.GetOptions();
				options.resolution = client;

				depthStencil_ = Sxx::Rendering::DepthStencilTexture::Create(options);
			}
		}
	}

	{
		const auto& keyboard = Sxx::Platform::Input::GetKeyboard();

		auto transform = camera_->GetComponent<Sxx::World::TransformComponent>();

		Vector3f position = transform->GetPosition();

		if (keyboard.GetKey(Sxx::Platform::KeyId::A).IsPress()) {
			position.x -= 0.01f;
		}

		if (keyboard.GetKey(Sxx::Platform::KeyId::D).IsPress()) {
			position.x += 0.01f;
		}

		if (keyboard.GetKey(Sxx::Platform::KeyId::W).IsPress()) {
			position.z += 0.01f;
		}

		if (keyboard.GetKey(Sxx::Platform::KeyId::S).IsPress()) {
			position.z -= 0.01f;
		}

		if (keyboard.GetKey(Sxx::Platform::KeyId::Q).IsPress()) {
			position.y += 0.01f;
		}

		if (keyboard.GetKey(Sxx::Platform::KeyId::E).IsPress()) {
			position.y -= 0.01f;
		}


		transform->SetPosition(position);
	}

	{
		auto transform = object_->GetComponent<Sxx::World::TransformComponent>();

		static const float rotationSpeed = 0.01f;
		transform->SetRotation(transform->GetRotation() * Quaternionf::AxisAngle(Vector3f::Up(), rotationSpeed));
	}

	Sxx::World::ComponentStorage::GetInstance()->ForEachActive<Sxx::World::TransformComponent>([](Sxx::World::TransformComponent* transform) {
		transform->Update();
	});
}

void SandboxUnit::RenderSandbox() {

	auto& context = Sxx::Graphics::Core::GetCommandContextDirect();

	{ //!< cacheの更新(Engine側での実行)
		// TODO: 別threadでcacheの更新を行うようにする.

		//!< CameraComponentのcacheの更新
		Sxx::World::ComponentStorage::GetInstance()->ForEachActive<Sxx::World::CameraComponent>([this](Sxx::World::CameraComponent* camera) {

			//!< cameraのcacheの更新
			Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::CameraCache>(camera->GetAddress(), *camera);

			RefPtr<Sxx::World::EntityBehaviour> owner = camera->GetOwner();

			auto transform = owner->GetComponent<Sxx::World::TransformComponent>();

			//!< transformのcacheの更新
			Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::TransformCache>(transform.GetAddress(), *transform);
			
		});

		//!< RendererComponentのcacheの更新
		Sxx::World::ComponentStorage::GetInstance()->ForEachActive<Sxx::World::StaticMeshRenderer>([this](Sxx::World::StaticMeshRenderer* renderer) {

			const Sxx::Assets::AssetHandle<Sxx::Assets::StaticMesh>& mesh          = renderer->GetMesh();
			const Sxx::Assets::AssetHandle<Sxx::Assets::SurfaceMaterial>& material = renderer->GetMaterial();

			//!< renderer側cacheの更新
			if (mesh.HasUuid()) {
				Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::StaticMeshCache>(mesh.GetUuid(), mesh.GetAddress(), mesh.Get());
			}

			if (material.HasUuid()) {
				Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::SurfaceMaterialCache>(material.GetUuid(), material.GetAddress(), material.Get());
			}

			RefPtr<Sxx::World::EntityBehaviour> owner = renderer->GetOwner();

			auto transform = owner->GetComponent<Sxx::World::TransformComponent>();

			//!< transformのcacheの更新
			Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::TransformCache>(transform.GetAddress(), *transform);
		});
	}

	{ //!< Sceneの更新(Engine側での実行)

		scene_.Reset();

		//!< RendererComponentのsceneへの追加
		Sxx::World::ComponentStorage::GetInstance()->ForEachActive<Sxx::World::StaticMeshRenderer>([this](Sxx::World::StaticMeshRenderer* renderer) {

			std::shared_ptr<const Sxx::Rendering::StaticMeshCache> mesh
				= Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::StaticMeshCache>(renderer->GetMesh().GetUuid());

			std::shared_ptr<const Sxx::Rendering::SurfaceMaterialCache> material
				= Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::SurfaceMaterialCache>(renderer->GetMaterial().GetUuid());

			RefPtr<Sxx::World::EntityBehaviour> owner = renderer->GetOwner();

			std::shared_ptr<const Sxx::Rendering::TransformCache> transform
				= Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::TransformCache>(owner->GetComponent<Sxx::World::TransformComponent>()->GetAddress());

			Sxx::Rendering::SceneProxy::StaticMeshInstance instance = {};
			instance.mesh      = mesh;
			instance.material  = material;
			instance.transform = transform;

			scene_.AddInstance(instance);
		});

		scene_.Setup();
	}

	{ //!< RenderTargetへの書き込み(Passとして書き込む)

		std::shared_ptr<const Sxx::Rendering::StaticMeshCache> meshCache
			= Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::StaticMeshCache>(object_->GetComponent<Sxx::World::StaticMeshRenderer>()->GetMesh().GetUuid());

		std::shared_ptr<const Sxx::Rendering::SurfaceMaterialCache> materialCache
			= Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::SurfaceMaterialCache>(object_->GetComponent<Sxx::World::StaticMeshRenderer>()->GetMaterial().GetUuid());

		std::shared_ptr<const Sxx::Rendering::CameraCache> cameraCache
			= Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::CameraCache>(camera_->GetComponent<Sxx::World::CameraComponent>().GetAddress());
		
		std::shared_ptr<const Sxx::Rendering::TransformCache> cameraTransformCache
			= Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::TransformCache>(camera_->GetComponent<Sxx::World::TransformComponent>().GetAddress());

		std::shared_ptr<const Sxx::Rendering::TransformCache> transformCache
			= Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::TransformCache>(object_->GetComponent<Sxx::World::TransformComponent>().GetAddress());

		if (meshCache->HasCache() && materialCache->HasCache()) {

			auto commandList = context.GetCommandList();

			renderTarget_.Transition(context, D3D12_RESOURCE_STATE_RENDER_TARGET);
			renderTarget_.ClearRenderTarget(context);

			depthStencil_.Transition(context, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			depthStencil_.ClearDepthStencil(context);

			commandList->OMSetRenderTargets(
				1, &renderTarget_.GetDescriptorRTV().GetCPUHandle(), false,
				&depthStencil_.GetDescriptorDSV().GetCPUHandle()
			);

			//D3D12_INDEX_BUFFER_VIEW ibv = meshCache->GetIndexBuffer().GetIndexBufferView();
			//commandList->IASetIndexBuffer(&ibv);

			pipeline0_.BindPipeline(context, renderTarget_.GetResolution());

			Sxx::Graphics::ShaderParameter parameter;
			parameter.SetAddress("gPositions", meshCache->GetPositionVertexBuffer().positions.GetGpuVirtualAddress());
			parameter.SetAddress("gVertices", meshCache->GetStaticMeshVertexBuffer().vertices.GetGpuVirtualAddress());
			parameter.SetAddress("gMeshlets", meshCache->GetMeshletBuffer().meshlets.GetGpuVirtualAddress());
			parameter.SetAddress("gVertexIndices", meshCache->GetMeshletBuffer().vertexIndices.GetGpuVirtualAddress());
			parameter.SetAddress("gTriangles", meshCache->GetMeshletBuffer().triangles.GetGpuVirtualAddress());
			parameter.Set32bitConstants("Information", meshCache->GetMeshletBuffer().meshletCount);
			parameter.SetAddress("gTransform", transformCache->GetTransformationBuffer().GetGpuVirtualAddress());
			parameter.SetAddress("gCameraProjection", cameraCache->GetProjectionBuffer().GetGpuVirtualAddress());
			parameter.SetAddress("gCameraTransform", cameraTransformCache->GetTransformationBuffer().GetGpuVirtualAddress());
			parameter.SetAddress("gMaterial", materialCache->GetMaterialBuffer().material.GetGpuVirtualAddress());

			pipeline0_.BindShaderParameter(context, parameter);

			//commandList->DrawIndexedInstanced(meshCache->GetIndexBuffer().GetIndexCount(), 1, 0, 0, 0);
			commandList->DispatchMesh(Sxx::Graphics::RoundUp(meshCache->GetMeshletBuffer().meshletCount, 32), 1, 1);

			renderTarget_.Transition(context, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			depthStencil_.Transition(context, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
	}

	if (Sxx::Framework::Context::HasUnit<Sxx::WindowUnit>()) {

		RefPtr<Sxx::WindowUnit> unit
			= Sxx::Framework::Context::GetUnit<Sxx::WindowUnit>(); //!< windowを管理しているunitを取得.

		auto& viewport = unit->GetViewport();

		viewport.BeginRenderPass(context, ColorCode4(0x9BA8A8FF));

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
