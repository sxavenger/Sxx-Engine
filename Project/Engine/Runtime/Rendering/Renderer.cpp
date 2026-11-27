#include "Renderer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering [cache]
#include "Cache/Static/StaticMeshCache.h"
#include "Cache/Static/SurfaceMaterialCache.h"
#include "Cache/Static/SkeletalMeshIntermediateCache.h"
#include "Cache/Dynamic/TransformCache.h"
#include "Cache/Dynamic/CameraCache.h"
#include "Cache/Dynamic/SkinnedMeshCache.h"
#include "Cache/Dynamic/DirectionalLightCache.h"
#include "Cache/Dynamic/PointLightCache.h"
#include "Cache/Dynamic/SkyLightCache.h"
#include "Cache/Manager/CacheCollection.h"

//* rendering [pass]
#include "Pass/RenderPassDeferredBase.h"
#include "Pass/RenderPassDirectLighting.h"
#include "Pass/RenderPassEnvironmentLighting.h"
#include "Pass/RenderPassCombineLighting.h"
#include "Pass/RenderPassAntiAliasing.h"
#include "Pass/RenderPassUpscaling.h"
#include "Pass/RenderPassTonemap.h"

//* rendering [skinning]
#include "Skinning/SkinningPipeline.h"

//* engine [graphics]
#include <Runtime/Graphics/Core.h>

//* engine [scheduler]
#include <Runtime/Scheduler/System.h>

//* engine [world]
#include <Runtime/World/Component/ComponentStorage.h>
#include <Runtime/World/Component/Transform/TransformComponent.h>
#include <Runtime/World/Component/Camera/CameraComponent.h>
#include <Runtime/World/Component/Renderer/StaticMeshRendererComponent.h>
#include <Runtime/World/Component/Renderer/SkeletalMeshRendererComponent.h>
#include <Runtime/World/Component/Light/Punctual/DirectionalLightComponent.h>
#include <Runtime/World/Component/Light/Punctual/PointLightComponent.h>
#include <Runtime/World/Component/Light/Environment/SkyLightComponent.h>
#include <Runtime/World/Entity/EntityBehaviour.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Renderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Renderer::Init() {

	//!< Passの初期化
	Renderer::InsertRenderPass<RenderPassDeferredBase>();
	Renderer::InsertRenderPass<RenderPassDirectLighting>();
	Renderer::InsertRenderPass<RenderPassEnvironmentLighting>();
	Renderer::InsertRenderPass<RenderPassCombineLighting>();
	Renderer::InsertRenderPass<RenderPassAntiAliasing>();
	Renderer::InsertRenderPass<RenderPassUpscaling>();
	Renderer::InsertRenderPass<RenderPassTonemap>();

	SkinningPipeline::Init(); //!< SkinningPipelineの初期化
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
	

	{ //!< StaticMeshRendererComponentのcacheの更新

		Scheduler::TaskHandle handle = Scheduler::System::PushCpuTask("StaticMeshRenderer Component Cache", Scheduler::TaskPriority::Priority::High, []() {
			Sxx::World::ComponentStorage::GetInstance()->ForEachActive<Sxx::World::StaticMeshRendererComponent>([](Sxx::World::StaticMeshRendererComponent* renderer) {

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

	{ //!< SkeletalMeshRendererComponentのcacheの更新

		Graphics::Core::GetCommandContextDirect().BeginEvent("SkeletalMeshRenderer Component Cache");

		Scheduler::TaskHandle handle = Scheduler::System::PushCpuTask("SkeletalMeshRenderer Component Cache", Scheduler::TaskPriority::Priority::High, []() {
			Sxx::World::ComponentStorage::GetInstance()->ForEachActive<Sxx::World::SkeletalMeshRendererComponent>([](Sxx::World::SkeletalMeshRendererComponent* renderer) {

				const Sxx::Assets::AssetHandle<Sxx::Assets::SkeletalMesh>& mesh        = renderer->GetMesh();
				const Sxx::Assets::AssetHandle<Sxx::Assets::SurfaceMaterial>& material = renderer->GetMaterial();

				//!< renderer側cacheの更新
				if (mesh.HasUuid()) {
					Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::SkeletalMeshIntermediateCache>(mesh.GetUuid(), mesh.GetAddress(), mesh.Get());
				}

				if (material.HasUuid()) {
					Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::SurfaceMaterialCache>(material.GetUuid(), material.GetAddress(), material.Get());
				}

				RefPtr<Sxx::World::EntityBehaviour> owner = renderer->GetOwner();

				auto transform = owner->GetComponent<Sxx::World::TransformComponent>();

				Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::TransformCache>(transform.GetAddress(), *transform);

				if (!mesh.HasUuid() || !Sxx::Rendering::CacheCollection::GetInstance()->HasCache(mesh.GetUuid())) {
					return; //!< intermediateが存在しない場合はskinned meshのcacheを作成しない (GetCacheはat()のため存在確認が必要)
				}

				std::shared_ptr<const Sxx::Rendering::SkeletalMeshIntermediateCache> intermediate
					= Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::SkeletalMeshIntermediateCache>(mesh.GetUuid());

				if (!intermediate->HasCache()) {
					return; //!< meshのtaskが完了していない場合
				}

				//!< skinned meshのcacheの更新 (bufferの作成, jointの解決, paletteの転送)
				Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::SkinnedMeshCache>(renderer->GetAddress(), *intermediate, *renderer);

				//!< attach jointのtransformのcacheの更新
				//!< skinning結果はattach joint local空間のため, 描画(VS/MS)とTLASのtransformはこちらを使用する.
				std::shared_ptr<Sxx::Rendering::SkinnedMeshCache> skinned
					= Sxx::Rendering::CacheCollection::GetInstance()->GetCache<Sxx::Rendering::SkinnedMeshCache>(renderer->GetAddress());

				skinned->Skinning(Graphics::Core::GetCommandContextDirect(), *intermediate);
				// TODO: Compute側で非同期に設定する.

				const RefPtr<const Sxx::World::EntityBehaviour>& attachJoint = skinned->GetAttachJointEntity();

				if (attachJoint == nullptr || !attachJoint->HasComponent<Sxx::World::TransformComponent>()) {
					return; //!< attach jointが無い場合はownerのtransformをfallbackとして使う (paletteはidentity)
				}

				RefPtr<const Sxx::World::TransformComponent> attachTransform = attachJoint->GetComponent<Sxx::World::TransformComponent>();

				if (attachTransform.GetAddress() != transform.GetAddress()) { //!< 単一mesh nodeではowner == attach jointのため重複を避ける
					Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::TransformCache>(attachTransform.GetAddress(), *attachTransform);
				}

			});
		});

		Graphics::Core::GetCommandContextDirect().EndEvent();

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

	{ //!< SkyLightComponentのcacheの更新
		Scheduler::TaskHandle handle = Scheduler::System::PushCpuTask("SkyLight Component Cache", Scheduler::TaskPriority::Priority::High, []() {
			Sxx::World::ComponentStorage::GetInstance()->ForEachActive<Sxx::World::SkyLightComponent>([](Sxx::World::SkyLightComponent* light) {
				//!< lightのcacheの更新
				Sxx::Rendering::CacheCollection::GetInstance()->Cache<Sxx::Rendering::SkyLightCache>(light->GetAddress(), *light);
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

