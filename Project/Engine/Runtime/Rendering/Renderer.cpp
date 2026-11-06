#include "Renderer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering [cache]
#include "Cache/Static/StaticMeshCache.h"
#include "Cache/Static/SurfaceMaterialCache.h"
#include "Cache/Dynamic/CameraCache.h"
#include "Cache/Dynamic/TransformCache.h"
#include "Cache/Dynamic/DirectionalLightCache.h"
#include "Cache/Manager/CacheCollection.h"

//* rendering [pass]
#include "Pass/RenderPassDeferredBase.h"
#include "Pass/RenderPassDirectLighting.h"
#include "Pass/RenderPassCombineLighting.h"
#include "Pass/RenderPassAntiAliasing.h"
#include "Pass/RenderPassUpscaling.h"
#include "Pass/RenderPassTonemap.h"

//* engine [scheduler]
#include <Runtime/Scheduler/System.h>

//* engine [world]
#include <Runtime/World/Component/ComponentStorage.h>
#include <Runtime/World/Component/Transform/TransformComponent.h>
#include <Runtime/World/Component/Camera/CameraComponent.h>
#include <Runtime/World/Component/Renderer/StaticMeshRenderer.h>
#include <Runtime/World/Component/Light/Punctual/DirectionalLightComponent.h>
#include <Runtime/World/Component/Light/Punctual/PointLightComponent.h>
#include <Runtime/World/Entity/EntityBehaviour.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Renderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Renderer::Init() {

	//!< Passの初期化
	Renderer::InsertRenderPass<RenderPassDeferredBase>();
	Renderer::InsertRenderPass<RenderPassDirectLighting>();
	Renderer::InsertRenderPass<RenderPassCombineLighting>();
	Renderer::InsertRenderPass<RenderPassAntiAliasing>();
	Renderer::InsertRenderPass<RenderPassUpscaling>();
	Renderer::InsertRenderPass<RenderPassTonemap>();

}

void Renderer::Cache() {

	{ //!< CameraComponentのcacheの更新
		Scheduler::TaskHandle handle = Scheduler::System::PushCpuTask("Camera Component Cache", Scheduler::TaskPriority::Priority::High, []() {
			World::ComponentStorage::GetInstance()->ForEachActive<World::CameraComponent>([](Sxx::World::CameraComponent* camera) {

				//!< cameraのcacheの更新
				Rendering::CacheCollection::GetInstance()->Cache<Rendering::CameraCache>(camera->GetAddress(), *camera);

				RefPtr<World::EntityBehaviour> owner = camera->GetOwner();

				auto transform = owner->GetComponent<World::TransformComponent>();

				//!< transformのcacheの更新
				Rendering::CacheCollection::GetInstance()->Cache<Rendering::TransformCache>(transform.GetAddress(), *transform);

			});
		});

		handle.Wait();
	}
	

	{ //!< RendererComponentのcacheの更新

		Scheduler::TaskHandle handle = Scheduler::System::PushCpuTask("StaticMeshRenderer Component Cache", Scheduler::TaskPriority::Priority::High, []() {
			Sxx::World::ComponentStorage::GetInstance()->ForEachActive<Sxx::World::StaticMeshRenderer>([](Sxx::World::StaticMeshRenderer* renderer) {

				const Sxx::Assets::AssetHandle<Sxx::Assets::StaticMesh>& mesh = renderer->GetMesh();
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
		});
		
		handle.Wait();
	}

	{ //!< DirectionalLightComponentのcacheの更新

		Scheduler::TaskHandle handle = Scheduler::System::PushCpuTask("DirectionalLight Component Cache", Scheduler::TaskPriority::Priority::High, []() {
			Sxx::World::ComponentStorage::GetInstance()->ForEachActive<Sxx::World::DirectionalLightComponent>([](Sxx::World::DirectionalLightComponent* light) {

				//!< lightのcacheの更新
				Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::DirectionalLightCache>(light->GetAddress(), *light);

				RefPtr<Sxx::World::EntityBehaviour> owner = light->GetOwner();

				auto transform = owner->GetComponent<Sxx::World::TransformComponent>();

				//!< transformのcacheの更新
				Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::TransformCache>(transform.GetAddress(), *transform);
			});
		});

		handle.Wait();
	}

	{ //!< PointLightComponentのcacheの更新
		Scheduler::TaskHandle handle = Scheduler::System::PushCpuTask("PointLight Component Cache", Scheduler::TaskPriority::Priority::High, []() {
			Sxx::World::ComponentStorage::GetInstance()->ForEachActive<Sxx::World::PointLightComponent>([](Sxx::World::PointLightComponent* light) {

				//!< lightのcacheの更新
				Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::PointLightCache>(light->GetAddress(), *light);

				RefPtr<Sxx::World::EntityBehaviour> owner = light->GetOwner();

				auto transform = owner->GetComponent<Sxx::World::TransformComponent>();

				//!< transformのcacheの更新
				Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::TransformCache>(transform.GetAddress(), *transform);
			});
		});

		handle.Wait();
	}

	// TODO: 完全に非同期で処理する.
}

void Renderer::Render(Graphics::GraphicsCommandContext& command, const PassContext& context) {
	command.CheckSupportType(Graphics::GraphicsCommandType::Direct); //!< DirectCommandContextであることを確認

	// TODO: SceneProxy等の設定は自動で行う.

	

	pipeline_.Execute(command, context);
}

