#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "BaseComponent.h"
#include "ComponentAllocator.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/CXXAttribute.hpp>
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/TypeInfo.h>

//* c++
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class ComponentStorage {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	//! @brief 各componentの格納コンテナを保持するコンテナ
	using Storage = std::unordered_map<TypeInfo, ComponentAllocator>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ComponentStorage() noexcept = default;
	~ComponentStorage() noexcept;

	//* component storage option *//

	//! @brief componentを登録
	template <Component T>
	NODISCARD ComponentHandle Register(RefPtr<EntityBehaviour> entity);

	//! @brief componentを登録解除
	template <Component T>
	void Unregister(ComponentHandle handle);

	//! @brief componentを登録解除
	void Unregister(const TypeInfo& type, ComponentHandle handle);

	//! @brief 登録解除されたcomponentの破棄
	void Destroy();

	//! @brief componentを取得
	template <Component T>
	RefPtr<T> GetComponent(ComponentHandle handle);

	//* singleton *//

	static ComponentStorage* GetInstance() noexcept;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* component storage *//

	Storage storage_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* component helper methods *//

	template <Component T>
	static RefPtr<T> Cast(RefPtr<BaseComponent> component);

};

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentStorage class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Component T>
inline ComponentHandle ComponentStorage::Register(RefPtr<EntityBehaviour> entity) {
	ComponentHandle handle = storage_[typeid(T)].Allocate(); //!< handleを生成
	StreamLogger::Debug("World::ComponentStorage<{}> | component allocate. handle: {}", typeid(T).name(), handle.GetHandle());

	storage_[typeid(T)].Register(std::make_unique<T>(handle.GetHandle(), entity));
	return handle;
}

template <Component T>
inline void ComponentStorage::Unregister(ComponentHandle handle) {
	if (!handle.HasHandle()) {
		StreamLogger::Warning("World::ComponentStorage<{}> | component handle has no handle.", typeid(T).name());
		return;
	}

	StreamLogger::Debug("World::ComponentStorage<{}> | component unregistered. handle: {}", typeid(T).name(), handle.GetHandle());
	storage_[typeid(T)].Unregister(handle);
}

template <Component T>
inline RefPtr<T> ComponentStorage::GetComponent(ComponentHandle handle) {
	return ComponentStorage::Cast<T>(storage_[typeid(T)].Get(handle)); //!< BaseComponentをTにキャストして返却.
}

template <Component T>
inline RefPtr<T> ComponentStorage::Cast(RefPtr<BaseComponent> component) {
	RefPtr<T> ptr = static_cast<T*>(component.Get());

	StreamLogger::Assert(ptr != nullptr, std::format("component cast failed. component type: {}", typeid(T).name()));
	return ptr;
}

SXAVENGER_ENGINE_NAMESPACE_END
