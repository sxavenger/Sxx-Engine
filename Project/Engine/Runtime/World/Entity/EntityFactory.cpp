#include "EntityFactory.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "EntityStorage.h"

//* engine
#include <Runtime/Assets/Bone/Bone.h>
#include <Runtime/Assets/Handle/AssetHandle.h>
#include <Runtime/World/Component/Transform/TransformComponent.h>
#include <Runtime/World/Component/Renderer/StaticMeshRendererComponent.h>
#include <Runtime/World/Component/Renderer/SkeletalMeshRendererComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// (anonymous namespace)
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	void InstantiateNode(EntityBehaviour* parent, const Assets::Bone::Node& node, const Assets::ModelContents& contents) {

		//!< entityを作成
		EntityHandle handle        = EntityFactory::Create();
		EntityBehaviour* behaviour = EntityFactory::GetEntity(handle);
		behaviour->SetName(node.name);

		//!< transformの追加
		behaviour->AddComponent<TransformComponent>();
		behaviour->GetComponent<TransformComponent>()->SetTransform(node.transform);

		//!< rendererの追加
		if (!node.staticMeshIds.empty() && !contents.skeletalMeshIds.empty()) {
			if (node.staticMeshIds.size() + contents.skeletalMeshIds.size() == 1) { //!< どちらかが1つだけの場合, そのentityにmeshを追加する.
				if (node.staticMeshIds.size() == 1) {

					//!< assetの取得
					Assets::AssetHandle<Assets::StaticMesh> mesh          = node.staticMeshIds.front();
					Assets::AssetHandle<Assets::SurfaceMaterial> material = contents.GetMeshMaterialId(mesh.GetUuid());

					//!< 名前の設定
					behaviour->SetName(mesh.GetName());

					//!< rendererの追加
					behaviour->AddComponent<StaticMeshRendererComponent>();
					auto renderer = behaviour->GetComponent<StaticMeshRendererComponent>();
					renderer->SetMesh(mesh);
					renderer->SetMaterial(material);

				} else if (contents.skeletalMeshIds.size() == 1) {

					//!< assetの取得
					Assets::AssetHandle<Assets::SkeletalMesh> mesh        = contents.skeletalMeshIds.front();
					Assets::AssetHandle<Assets::SurfaceMaterial> material = contents.GetMeshMaterialId(mesh.GetUuid());

					//!< 名前の設定
					behaviour->SetName(mesh.GetName());

					behaviour->AddComponent<SkeletalMeshRendererComponent>();
					auto renderer = behaviour->GetComponent<SkeletalMeshRendererComponent>();
					renderer->SetMesh(mesh);
					renderer->SetMaterial(material);
					
				}
			} else {
				for (const auto& meshId : node.staticMeshIds) {

					EntityHandle child      = EntityFactory::Create(); //!< entityを作成
					EntityBehaviour* entity = EntityFactory::GetEntity(child); //!< entityを取得

					//!< transformの追加
					entity->AddComponent<TransformComponent>();

					//!< assetの取得
					Assets::AssetHandle<Assets::StaticMesh> mesh          = meshId;
					Assets::AssetHandle<Assets::SurfaceMaterial> material = contents.GetMeshMaterialId(mesh.GetUuid());

					//!< 名前の設定
					entity->SetName(mesh.GetName());

					//!< rendererの追加
					entity->AddComponent<StaticMeshRendererComponent>();
					auto renderer = entity->GetComponent<StaticMeshRendererComponent>();
					renderer->SetMesh(mesh);
					renderer->SetMaterial(material);

					//!< entityを親に追加
					behaviour->AttachChild(std::move(child));
				}

				for (const auto& meshId : node.skeletalMeshIds) {
					//!< entityの作成
					EntityHandle child      = EntityFactory::Create();
					EntityBehaviour* entity = EntityFactory::GetEntity(child);

					//!< transformの追加
					entity->AddComponent<TransformComponent>();
					entity->GetComponent<TransformComponent>()->SetTransform(node.transform);

					//!< assetの取得
					Assets::AssetHandle<Assets::SkeletalMesh> mesh        = meshId;
					Assets::AssetHandle<Assets::SurfaceMaterial> material = contents.GetMeshMaterialId(mesh.GetUuid());

					//!< 名前の設定
					entity->SetName(mesh.GetName());

					entity->AddComponent<SkeletalMeshRendererComponent>();
					auto renderer = entity->GetComponent<SkeletalMeshRendererComponent>();
					renderer->SetMesh(mesh);
					renderer->SetMaterial(material);

					//!< entityを親に追加
					behaviour->AttachChild(std::move(child));
				}
			}

		}
		
		parent->AttachChild(std::move(handle)); //!< entityを親に追加

		for (const auto& child : node.children) {
			InstantiateNode(behaviour, child, contents);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// EntityFactory class methods
////////////////////////////////////////////////////////////////////////////////////////////

EntityHandle EntityFactory::Create() {
	return EntityStorage::GetInstance()->Register();
}

void EntityFactory::Destroy(const EntityHandle& handle) {
	EntityStorage::GetInstance()->Unregister(handle);
}

RefPtr<EntityBehaviour> EntityFactory::GetEntity(const EntityHandle& handle) {
	return EntityStorage::GetInstance()->GetEntity(handle);
}

EntityHandle EntityFactory::CreateModelEntity(const Assets::ModelContents& contents) {

	Assets::AssetHandle<Assets::Bone> bone = contents.boneId;

	EntityHandle handle = EntityFactory::Create();
	InstantiateNode(EntityFactory::GetEntity(handle), bone.WaitGet()->GetDescription().root, contents);

	return handle;
}
