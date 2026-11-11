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
#include <Runtime/World/Component/Renderer/StaticMeshRenderer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// (anonymous namespace)
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	void StaticMeshNode(EntityBehaviour* parent, const Assets::Bone& bone, const Assets::ModelContents& contents) {

		//!< entityを作成
		EntityHandle handle        = EntityFactory::Create();
		EntityBehaviour* behaviour = EntityFactory::GetEntity(handle);

		const Assets::Bone::Description& description = bone.GetDescription();

		//!< transformの追加
		behaviour->AddComponent<TransformComponent>();
		behaviour->GetComponent<TransformComponent>()->SetTransform(description.transform);

		//!< rendererの追加
		if (!description.meshIds.empty()) {
			if (description.meshIds.size() == 1) {

				//!< assetの取得
				Assets::AssetHandle<Assets::StaticMesh> mesh          = description.meshIds.front();
				Assets::AssetHandle<Assets::SurfaceMaterial> material = contents.GetMeshMaterialId(mesh.GetUuid());

				//!< 名前の設定
				behaviour->SetName(mesh.GetName());

				//!< rendererの追加
				behaviour->AddComponent<StaticMeshRenderer>();
				auto renderer = behaviour->GetComponent<StaticMeshRenderer>();
				renderer->SetMesh(mesh.GetUuid());
				renderer->SetMaterial(material.GetUuid());

			} else {
				for (const auto& meshId : description.meshIds) {

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
					entity->AddComponent<StaticMeshRenderer>();
					auto renderer = entity->GetComponent<StaticMeshRenderer>();
					renderer->SetMesh(mesh.GetUuid());
					renderer->SetMaterial(material.GetUuid());

					//!< entityを親に追加
					behaviour->AttachChild(std::move(child));
				}
			}
		}
		
		parent->AttachChild(std::move(handle)); //!< entityを親に追加
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

	Sxx::Assets::AssetHandle<Sxx::Assets::Bone> bone = contents.boneId;

	EntityHandle handle = EntityFactory::Create();
	StaticMeshNode(EntityFactory::GetEntity(handle), *bone.WaitGet(), contents);

	return handle;
}
