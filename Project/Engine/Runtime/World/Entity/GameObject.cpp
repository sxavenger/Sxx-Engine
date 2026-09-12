#include "GameObject.h"
#include "GameObject.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "EntityStorage.h"

//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [GameObject] Instance structure methods
////////////////////////////////////////////////////////////////////////////////////////////

GameObject::Instance::Instance() {
	//!< entityを作成
	handle_ = EntityStorage::GetInstance()->Register();
}

GameObject::Instance::Instance(const std::string_view& name) {
	//!< entityを作成
	handle_ = EntityStorage::GetInstance()->Register();

	//!< entityの名前を設定
	RefPtr<EntityBehaviour> entity = EntityStorage::GetInstance()->GetEntity(handle_);
	entity->SetName(name);
}

GameObject::Instance::~Instance() {
	if (handle_.HasHandle()) {
		//!< entityを破棄
		EntityStorage::GetInstance()->Unregister(handle_);
	}
}

RefPtr<EntityBehaviour> GameObject::Instance::GetEntity() const {
	return EntityStorage::GetInstance()->GetEntity(handle_);
}

////////////////////////////////////////////////////////////////////////////////////////////
// GameObject class methods
////////////////////////////////////////////////////////////////////////////////////////////

GameObject::GameObject() {
	instance_ = std::make_unique<Instance>();
}

GameObject::~GameObject() {
	instance_.reset();
}

RefPtr<EntityBehaviour> GameObject::GetEntity() const {
	STREAM_ASSERT(instance_ != nullptr, "GameObject instance is null.");
	return instance_->GetEntity();
}
