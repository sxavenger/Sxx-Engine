#include "GameObject.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "EntityBehaviour.h"
#include "EntityStorage.h"

//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameObject class methods
////////////////////////////////////////////////////////////////////////////////////////////

GameObject::~GameObject() {
	Reset();
}

void GameObject::Reset() {
	if (handle_.HasHandle()) {
		EntityStorage::GetInstance()->Unregister(std::exchange(handle_, std::nullopt)); //!< entityを破棄
	}
}

NODISCARD EntityHandle GameObject::Release() noexcept {
	if (!handle_.HasHandle()) {
		return std::nullopt;
	}

	EntityHandle handle = std::exchange(handle_, std::nullopt); //!< entityの所有権を放棄
	return handle;
}

RefPtr<EntityBehaviour> GameObject::GetEntity() const {
	STREAM_ASSERT(handle_.HasHandle(), "GameObject has no entity.");
	return EntityStorage::GetInstance()->GetEntity(handle_);
}

GameObject::GameObject(GameObject&& other) noexcept {
	Reset(); //!< 既存のentityを破棄
	handle_ = std::exchange(other.handle_, std::nullopt); //!< otherのentityの所有権を移動
}

GameObject& GameObject::operator=(GameObject&& other) noexcept {
	Reset(); //!< 既存のentityを破棄
	handle_ = std::exchange(other.handle_, std::nullopt); //!< otherのentityの所有権を移動
	return *this;
}

GameObject::GameObject(EntityHandle&& handle) noexcept {
	Reset(); //!< 既存のentityを破棄
	handle_ = std::exchange(handle, std::nullopt); //!< handleの所有権を移動
}

GameObject& GameObject::operator=(EntityHandle&& handle) noexcept {
	Reset(); //!< 既存のentityを破棄
	handle_ = std::exchange(handle, std::nullopt); //!< handleの所有権を移動
	return *this;
}

GameObject GameObject::Create() {
	return EntityStorage::GetInstance()->Register(); //!< entityを作成
}

GameObject GameObject::Create(const std::string_view& name) {
	EntityHandle handle = EntityStorage::GetInstance()->Register(); //!< entityを作成

	RefPtr<EntityBehaviour> entity = EntityStorage::GetInstance()->GetEntity(handle);
	entity->SetName(name); //!< entityの名前を設定

	return handle;
}
