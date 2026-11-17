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
#include <Runtime/World/Component/Armature/ArmatureComponent.h>
#include <Runtime/World/Component/Armature/ArmatureJointComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// (anonymous namespace)
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	void InstantiateNode(EntityBehaviour* parent, const Assets::Bone::Node& node, const Assets::ModelContents& contents, const bool isArmature) {

		//!< entityの生成
		EntityHandle handle     = EntityFactory::Create();
		EntityBehaviour* entity = EntityFactory::GetEntity(handle);
		entity->SetName(node.name);
		
		{ //!< transformの設定
			auto transform = entity->AddComponent<TransformComponent>();
			transform->SetTransform(node.transform);
		}

		if (isArmature) { //!< Armatureを使用する場合, ArmatureJointComponentを追加
			entity->AddComponent<ArmatureJointComponent>();
			entity->GetComponent<ArmatureJointComponent>()->SetName(node.name);
		}

		const size_t nodeMeshCount = node.staticMeshIds.size() + node.skeletalMeshIds.size(); //!< メッシュの数
		if (nodeMeshCount == 1) {
			//!< meshが1つの場合, 不要なTreeNodeを生成せず, 直接MeshRendererComponentを追加する.

			if (node.staticMeshIds.size() == 1) {

				//!< assetの取得
				Assets::AssetHandle<Assets::StaticMesh> mesh          = node.staticMeshIds.front();
				Assets::AssetHandle<Assets::SurfaceMaterial> material = contents.GetMeshMaterialId(mesh.GetUuid());

				//!< 名前の変更
				entity->SetName(mesh.GetName());

				//!< MeshRendererComponentの追加
				auto renderer = entity->AddComponent<StaticMeshRendererComponent>();
				renderer->SetMesh(mesh);
				renderer->SetMaterial(material);

			} else if (node.skeletalMeshIds.size() == 1) {

				//!< assetの取得
				Assets::AssetHandle<Assets::SkeletalMesh> mesh        = node.skeletalMeshIds.front();
				Assets::AssetHandle<Assets::SurfaceMaterial> material = contents.GetMeshMaterialId(mesh.GetUuid());

				//!< 名前の変更
				entity->SetName(mesh.GetName());

				//!< MeshRendererComponentの追加
				auto renderer = entity->AddComponent<SkeletalMeshRendererComponent>();
				renderer->SetMesh(mesh);
				renderer->SetMaterial(material);
				renderer->SetJointName(node.name); //!< joint名を設定
				
			}
			
		} else if (nodeMeshCount > 1) {
			//!< meshが複数ある場合, 子としてMeshRendererComponentを追加する.

			for (const auto& meshId : node.staticMeshIds) {

				//!< assetの取得
				Assets::AssetHandle<Assets::StaticMesh> mesh          = meshId;
				Assets::AssetHandle<Assets::SurfaceMaterial> material = contents.GetMeshMaterialId(mesh.GetUuid());

				//!< entityの生成
				EntityHandle child           = EntityFactory::Create();
				EntityBehaviour* childEntity = EntityFactory::GetEntity(child);
				childEntity->SetName(mesh.GetName());

				//!< transformの設定
				childEntity->AddComponent<TransformComponent>();

				//!< MeshRendererComponentの追加
				auto renderer = childEntity->AddComponent<StaticMeshRendererComponent>();
				renderer->SetMesh(mesh);
				renderer->SetMaterial(material);

				//!< 親子関係の設定
				entity->AttachChild(std::move(child));
			}

			for (const auto& meshId : node.skeletalMeshIds) {

				//!< assetの取得
				Assets::AssetHandle<Assets::SkeletalMesh> mesh        = meshId;
				Assets::AssetHandle<Assets::SurfaceMaterial> material = contents.GetMeshMaterialId(mesh.GetUuid());

				//!< entityの生成
				EntityHandle child           = EntityFactory::Create();
				EntityBehaviour* childEntity = EntityFactory::GetEntity(child);
				childEntity->SetName(mesh.GetName());

				//!< transformの設定
				childEntity->AddComponent<TransformComponent>();

				//!< MeshRendererComponentの追加
				auto renderer = childEntity->AddComponent<SkeletalMeshRendererComponent>();
				renderer->SetMesh(mesh);
				renderer->SetMaterial(material);
				renderer->SetJointName(node.name); //!< joint名を設定

				//!< 親子関係の設定
				entity->AttachChild(std::move(child));
			}
		}

		parent->AttachChild(std::move(handle)); //!< 親子関係の設定

		for (const auto& child : node.children) {
			InstantiateNode(entity, child, contents, isArmature);
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

	const bool isArmature = !contents.skeletalMeshIds.empty(); //!< Armatureを使用するかどうかの判定

	EntityHandle handle        = EntityFactory::Create();
	EntityBehaviour* behaviour = EntityFactory::GetEntity(handle);

	behaviour->AddComponent<TransformComponent>();

	if (isArmature) { //!< Armatureを使用する場合, ArmatureComponentを追加
		behaviour->AddComponent<ArmatureComponent>();
		behaviour->GetComponent<ArmatureComponent>()->SetBone(bone);
	}
	
	InstantiateNode(behaviour, bone.WaitGet()->GetDescription().root, contents, isArmature);

	return handle;
}
