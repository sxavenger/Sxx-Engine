#include "EntityBehaviour.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "EntityStorage.h"
#include "GameObject.h"

//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EntityBehaviour class methods
////////////////////////////////////////////////////////////////////////////////////////////

EntityBehaviour::~EntityBehaviour() {
	//!< componentの登録解除
	for (auto& [type, handle] : components_) {
		ComponentStorage::GetInstance()->Unregister(type, handle);
	}

	
	{ //!< 親からの切り離し
		if (parent_.HasHandle()) {
			RefPtr<EntityBehaviour> parent = EntityStorage::GetInstance()->GetEntity(parent_);
			parent->children_.erase(handle_);
		}
	}

	
	{ //!< 子の解放と切り離し
		for (auto& [handle, ownership] : children_) {
			RefPtr<EntityBehaviour> child = EntityStorage::GetInstance()->GetEntity(handle);
			child->parent_ = std::nullopt;

			if (ownership == Ownership::Owned) {
				EntityStorage::GetInstance()->Unregister(handle); //!< 所有している子の解放
			}
		}

		children_.clear();
	}
	
}

void EntityBehaviour::SetActive(bool active) {

	isActive_ = active;

	for (const auto& handle : children_ | std::views::keys) {
		RefPtr<EntityBehaviour> child = EntityStorage::GetInstance()->GetEntity(handle);
		child->SetActive(active);
	}
}

RefPtr<EntityBehaviour> EntityBehaviour::GetParent() const {
	return EntityStorage::GetInstance()->GetEntity(parent_);
}

void EntityBehaviour::AttachChild(GameObject&& child) {
	AttachInternal(child.Release(), Ownership::Owned); //!< 所有権ごと子を渡す
}

void EntityBehaviour::AttachChild(EntityHandle&& child) {
	AttachInternal(std::move(child), Ownership::Owned); //!< 所有権ごと子を渡す
}

void EntityBehaviour::AttachChild(const EntityHandle& child) {
	AttachInternal(child, Ownership::Borrowed); //!< 借用として子を追加
}

void EntityBehaviour::AttachInternal(const EntityHandle& child, Ownership ownership) {
	STREAM_ASSERT(child != handle_, "cannot attach self."); //!< thisを子として追加することはできない.

	RefPtr<EntityBehaviour> entity = EntityStorage::GetInstance()->GetEntity(child);

	if (entity->HasParent()) {
		STREAM_LOG_WARNING("World::EntityBehaviour | child already has a parent. detach it first. child: {}", child.GetHandle());
	}

	//!< 階層関係の設定
	entity->parent_ = handle_;
	children_.emplace(child, ownership);
}
