#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "EntityHandle.h"

//* component
#include "../Component/ComponentHandle.h"
#include "../Component/BaseComponent.h"
#include "../Component/ComponentStorage.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/TypeInfo.h>

//* c++
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class GameObject;

////////////////////////////////////////////////////////////////////////////////////////////
// EntityBehaviour class
////////////////////////////////////////////////////////////////////////////////////////////
class EntityBehaviour final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// OwnerShip enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Ownership : bool {
		Owned,
		Borrowed
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mobility enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Mobility : uint8_t {
		Static,
		Stationary,
		Movable
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	EntityBehaviour(uint64_t handle) noexcept : handle_(handle) {}
	~EntityBehaviour();

	//* entity option *//

	void SetName(const std::string_view& name) { name_ = name; }

	std::string_view GetName() const noexcept { return name_; }

	bool IsActive() const noexcept { return isActive_; }

	void SetActive(bool active);

	//* hierarchy option *//

	bool HasParent() const noexcept { return parent_.HasHandle(); }

	RefPtr<EntityBehaviour> GetParent() const;

	bool HasChildren() const noexcept { return !children_.empty(); }

	//! @brief 所有権ごと子を渡す
	void AttachChild(GameObject&& child);

	//! @brief 所有権ごと子を渡す
	void AttachChild(EntityHandle&& child);

	//! @brief 借用として子を追加
	void AttachChild(const EntityHandle& child);

	const std::unordered_map<EntityHandle, Ownership, EntityHandle::Hash>& GetChildren() const noexcept { return children_; }

	//* component option *//

	//! @brief componentを追加
	template <Component T>
	void AddComponent();

	//! @brief componentを削除
	template <Component T>
	void RemoveComponent();

	template <Component T>
	bool HasComponent() const;

	//! @brief componentを取得
	template <Component T>
	RefPtr<T> GetComponent();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* handle *//

	const EntityHandle handle_;

	//* hierarchy *//

	EntityHandle parent_ = std::nullopt;
	std::unordered_map<EntityHandle, Ownership, EntityHandle::Hash> children_;

	//* component *//

	std::unordered_map<TypeInfo, ComponentHandle> components_;

	//* entity *//

	std::string name_ = "Unnamed Entity";

	Mobility mobility_ = Mobility::Movable;

	bool isActive_ = true;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void AttachInternal(const EntityHandle& child, Ownership ownership);

};

////////////////////////////////////////////////////////////////////////////////////////////
// EntityBehaviour class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Component T>
inline void EntityBehaviour::AddComponent() {
	constexpr const std::type_info* type = &typeid(T);

	if (components_.contains(type)) {
		STREAM_LOG_WARNING("World::EntityBehaviour | component already exists. type: ", type->name());
		//!< すでにcomponentが存在する場合は追加しない.
		return;
	}

	//!< componentの追加
	components_[type] = ComponentStorage::GetInstance()->Register<T>(this);
}

template <Component T>
inline void EntityBehaviour::RemoveComponent() {
	constexpr const std::type_info* type = &typeid(T);

	if (!components_.contains(type)) {
		STREAM_LOG_WARNING("World::EntityBehaviour | component not found. type: ", type->name());
		//!< componentが存在しない場合は削除しない.
		return;
	}

	//!< componentの削除
	ComponentStorage::GetInstance()->Unregister(components_[type]);
	components_.erase(type);
}

template <Component T>
inline bool EntityBehaviour::HasComponent() const {
	constexpr const std::type_info* type = &typeid(T);
	return components_.contains(type);
}

template <Component T>
inline RefPtr<T> EntityBehaviour::GetComponent() {
	constexpr const std::type_info* type = &typeid(T);

	if (!components_.contains(type)) {
		STREAM_LOG_WARNING("World::EntityBehaviour | component not found. type: ", type->name());
		//!< componentが存在しない場合は取得しない.
		return nullptr;
	}

	//!< componentの取得
	return ComponentStorage::GetInstance()->GetComponent<T>(components_[type]);
}

SXAVENGER_ENGINE_NAMESPACE_END
