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
#include <Engine/Runtime/World/Component/Light/Punctual/DirectionalLightComponent.h>

//* engine [rendering]
#include <Engine/Runtime/Rendering/Cache/Static/StaticMeshCache.h>
#include <Engine/Runtime/Rendering/Cache/Static/SurfaceMaterialCache.h>
#include <Engine/Runtime/Rendering/Cache/Dynamic/CameraCache.h>
#include <Engine/Runtime/Rendering/Cache/Dynamic/TransformCache.h>
#include <Engine/Runtime/Rendering/Cache/Dynamic/DirectionalLightCache.h>
#include <Engine/Runtime/Rendering/Cache/Manager/CacheCollection.h>
#include <Engine/Runtime/Rendering/Pass/RenderPassDeferredBase.h>
#include <Engine/Runtime/Rendering/Pass/RenderPassDirectLighting.h>
#include <Engine/Runtime/Rendering/Pass/RenderPassTonemap.h>
#include <Engine/Runtime/Rendering/Buffer/LightingBuffer.h>

//* engine [editor]
#include <Engine/Runtime/Editor/ViewportPanel.h>

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
	Sxx::Framework::Context::Push<Sxx::SlateEditorUnit>();
	//Sxx::Framework::Context::Push<Sxx::WindowUnit>();
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

	{ //!< Lightの作成
		light_->AddComponent<Sxx::World::TransformComponent>();
		light_->AddComponent<Sxx::World::DirectionalLightComponent>();

		auto transform = light_->GetComponent<Sxx::World::TransformComponent>();
		//transform->SetRotation();

		auto light = light_->GetComponent<Sxx::World::DirectionalLightComponent>();
		light->SetColor({ 1.0f, 1.0f, 1.0f });
		light->SetIntensity(3.14f);
	}

	{ //!< Passの初期化

		pipeline_.Insert<Sxx::Rendering::RenderPassDeferredBase>();
		pipeline_.Insert<Sxx::Rendering::RenderPassDirectLighting>();
		pipeline_.Insert<Sxx::Rendering::RenderPassTonemap>();

	}

	// TODO: Engine側で行う
	if (Sxx::Framework::Context::HasUnit<Sxx::SlateEditorUnit>()) {
		auto viewport = Sxx::Framework::Context::GetUnit<Sxx::SlateEditorUnit>()->GetPanel<Sxx::Editor::ViewportPanel>();
		viewport->SetBufferCollection(&buffer_);
	}
}

void SandboxUnit::TermSandbox() {
}

void SandboxUnit::UpdateSandbox() {

	const auto& keyboard = Sxx::Platform::Input::GetKeyboard();
	const auto& mouse    = Sxx::Platform::Input::GetMouse();

	{

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

	{
		static uint32_t frameCount = 0;

		if (frameCount++ % 300 == 0) { //!< 5秒ごとにWorkingColorSpaceを切り替える
			switch (option_.workingColorSpace) {
				case Sxx::Rendering::PassOption::WorkingColorSpace::Linear:
					option_.workingColorSpace = Sxx::Rendering::PassOption::WorkingColorSpace::ACEScg;
					STREAM_LOG_DEBUG("WorkingColorSpace changed to ACEScg");
					break;

				case Sxx::Rendering::PassOption::WorkingColorSpace::ACEScg:
					option_.workingColorSpace = Sxx::Rendering::PassOption::WorkingColorSpace::Linear;
					STREAM_LOG_DEBUG("WorkingColorSpace changed to Linear");
					break;
			}
		}
	}

	{

		if (mouse.IsWheel()) {
			auto light = light_->GetComponent<Sxx::World::DirectionalLightComponent>();
			float intensity = light->GetIntensity();
			intensity = std::max(0.0f, intensity + mouse.GetDeltaWheelNormalized() * 0.1f);
			light->SetIntensity(intensity);
		}
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

		//!< DirectionalLightComponentのcacheの更新
		Sxx::World::ComponentStorage::GetInstance()->ForEachActive<Sxx::World::DirectionalLightComponent>([this](Sxx::World::DirectionalLightComponent* light) {

			//!< lightのcacheの更新
			Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::DirectionalLightCache>(light->GetAddress(), *light);

			RefPtr<Sxx::World::EntityBehaviour> owner = light->GetOwner();

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

			scene_.AddMeshInstance(instance);
		});

		scene_.Setup();

		//!< DirectionalLightComponentのsceneへの追加
		Sxx::World::ComponentStorage::GetInstance()->ForEachActive<Sxx::World::DirectionalLightComponent>([this](Sxx::World::DirectionalLightComponent* light) {
			std::shared_ptr<const Sxx::Rendering::DirectionalLightCache> lightCache
				= Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::DirectionalLightCache>(light->GetAddress());

			RefPtr<Sxx::World::EntityBehaviour> owner = light->GetOwner();

			std::shared_ptr<const Sxx::Rendering::TransformCache> transform
				= Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::TransformCache>(owner->GetComponent<Sxx::World::TransformComponent>()->GetAddress());

			Sxx::Rendering::SceneProxy::DirectionalLightInstance instance = {};
			instance.light     = lightCache;
			instance.transform = transform;

			scene_.AddLightInstance(instance);
		});
	}

	{ //!< Passの実行

		Sxx::Rendering::PassContext x = {};
		x.buffer = &buffer_;
		x.scene  = &scene_;
		x.camera.camera    = Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::CameraCache>(camera_->GetComponent<Sxx::World::CameraComponent>().GetAddress());
		x.camera.transform = Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::TransformCache>(camera_->GetComponent<Sxx::World::TransformComponent>().GetAddress());
		x.resolution       = Sxx::Framework::Context::HasUnit<Sxx::WindowUnit>() ? Sxx::Framework::Context::GetUnit<Sxx::WindowUnit>()->GetViewport().GetClient() : Vector2u(1280, 720);
		x.option           = option_;

		pipeline_.Execute(context, x);

	}

	if (Sxx::Framework::Context::HasUnit<Sxx::WindowUnit>()) {

		RefPtr<Sxx::WindowUnit> unit
			= Sxx::Framework::Context::GetUnit<Sxx::WindowUnit>(); //!< windowを管理しているunitを取得.

		auto& viewport = unit->GetViewport();

		viewport.BeginRenderPass(context, ColorCode4(0x9BA8A8FF));

#if 1

		{ //!< Shaderでの書き込み

			auto commandList = context.GetCommandList();

			pipeline1_.BindPipeline(context, viewport.GetClient());

			Sxx::Graphics::ShaderParameter parameter;
			parameter.SetHandle("gTexture", buffer_.GetInstance<Sxx::Rendering::LightingBuffer>().GetBuffer(Sxx::Rendering::LightingBuffer::Layout::Direct).GetDescriptorSRV().GetGPUHandle());

			pipeline1_.BindShaderParameter(context, parameter);

			commandList->DrawInstanced(3, 1, 0, 0);
		}
#endif

		viewport.EndRenderPass(context);
	}
}
