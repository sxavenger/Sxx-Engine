#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "EntityHandle.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/CXXAttribute.hpp>
#include <Lib/Pointer/ReferencePointer.h>

//* c++
#include <memory>
#include <string_view>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class EntityBehaviour;

////////////////////////////////////////////////////////////////////////////////////////////
// GameObject class
////////////////////////////////////////////////////////////////////////////////////////////
class GameObject final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	GameObject() noexcept = default;
	~GameObject();

	//* game object option *//

	//! @brief 
	void Reset();

	//! @brief
	NODISCARD EntityHandle Release() noexcept;

	//* entity option *//

	RefPtr<EntityBehaviour> GetEntity() const;

	//* operator [access] *//

	EntityBehaviour* operator->() { return GetEntity(); }
	const EntityBehaviour* operator->() const { return GetEntity(); }

	//* operator [copy] <GameObject> (delete) *//

	GameObject(const GameObject&)            = delete;
	GameObject& operator=(const GameObject&) = delete;

	//* operator [move] <GameObject> *//

	GameObject(GameObject&& other) noexcept;
	GameObject& operator=(GameObject&& other) noexcept;

	//* operator [move] <EntityHandle> *//

	GameObject(EntityHandle&& handle) noexcept;
	GameObject& operator=(EntityHandle&& handle) noexcept;

	//* static methods *//

	static GameObject Create();

	static GameObject Create(const std::string_view& name);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	EntityHandle handle_ = std::nullopt;

};

SXAVENGER_ENGINE_NAMESPACE_END
