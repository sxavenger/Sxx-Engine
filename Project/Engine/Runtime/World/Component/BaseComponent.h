#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "ComponentHandle.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Pointer/ReferencePointer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class EntityBehaviour; //!< Componentが所属するEntity

////////////////////////////////////////////////////////////////////////////////////////////
// BaseComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief componentの基底クラス
class BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	BaseComponent(uint64_t handle, RefPtr<EntityBehaviour> owner) noexcept : handle_(handle), owner_(owner) {}
	virtual ~BaseComponent() = default;

	//* component option *//

	virtual bool IsActive() const;

	const ComponentHandle& GetHandle() const noexcept { return handle_; }

	uintptr_t GetAddress() const noexcept { return reinterpret_cast<uintptr_t>(this); }

	//* entity option *//

	const RefPtr<EntityBehaviour> GetOwner() const noexcept { return owner_; }

	//* serialization option *//

	// TODO: serializationの実装を追加する

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	const ComponentHandle handle_;

	//* entity *//

	const RefPtr<EntityBehaviour> owner_ = nullptr; //!< componentが所属するentity

};

//-----------------------------------------------------------------------------------------
// concepts
//-----------------------------------------------------------------------------------------
template <typename T>
concept Component = std::derived_from<T, BaseComponent>;

SXAVENGER_ENGINE_NAMESPACE_END
