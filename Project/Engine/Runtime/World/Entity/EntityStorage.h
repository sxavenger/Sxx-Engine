#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "EntityHandle.h"
#include "EntityBehaviour.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// EntityStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class EntityStorage final {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Storage = std::unordered_map<EntityHandle, std::unique_ptr<EntityBehaviour>, EntityHandle::Hash>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	EntityStorage() noexcept;
	~EntityStorage() noexcept;

	//* entity storage option *//

	EntityHandle Register();

	void Unregister(EntityHandle handle);

	void Destroy();

	RefPtr<EntityBehaviour> GetEntity(EntityHandle handle) const;

	// TODO: 名前からの取得機能を追加する

	//* singleton *//

	static EntityStorage* GetInstance() noexcept;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* entity storage *//

	Storage storage_;

	IndexAllocator<EntityHandle::Type> allocator_;
	std::queue<EntityHandle> unregister_;

};

SXAVENGER_ENGINE_NAMESPACE_END
