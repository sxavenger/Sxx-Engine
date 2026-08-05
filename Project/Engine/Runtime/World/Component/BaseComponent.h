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

	BaseComponent(uint64_t handle, RefPtr<EntityBehaviour> entity) noexcept : handle_(handle), entity_(entity) {}
	virtual ~BaseComponent() = default;

	//* component option *//

	const ComponentHandle& GetHandle() const noexcept { return handle_; }

	//* entity option *//

	const RefPtr<EntityBehaviour> GetEntity() const noexcept { return entity_; }

	//* serialization option *//

	// TODO: serializationの実装を追加する

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	const ComponentHandle handle_;

	//* entity *//

	const RefPtr<EntityBehaviour> entity_ = nullptr; //!< componentが所属するEntity

};

//-----------------------------------------------------------------------------------------
// Component concept
//-----------------------------------------------------------------------------------------
template <typename T>
concept Component = std::derived_from<T, BaseComponent>;

SXAVENGER_ENGINE_NAMESPACE_END
