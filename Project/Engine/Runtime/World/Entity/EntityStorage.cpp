#include "EntityStorage.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <limits>

////////////////////////////////////////////////////////////////////////////////////////////
// EntityStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

EntityStorage::EntityStorage() noexcept {
	//!< entityの最大数を設定.
	allocator_.Capacity(std::numeric_limits<EntityHandle::Type>::max()); 
}

EntityStorage::~EntityStorage() noexcept {

	size_t used = allocator_.GetUsedCount();

	if (used != 0) {
		STREAM_LOG_WARNING("World::EntityStorage | entity handle reaked. count: {}", allocator_.GetUsedCount());
		//!< entityのhandleの解放がされていない場合は警告を出す.
	}
}

EntityHandle EntityStorage::Register() {
	EntityHandle handle = EntityHandle(allocator_.Allocate());
	storage_.emplace(handle, std::make_unique<EntityBehaviour>(handle.GetHandle()));

	STREAM_LOG_DEBUG("World::EntityStorage | entity registered. handle: {}", handle.GetHandle());
	return handle;
}

void EntityStorage::Unregister(EntityHandle handle) {
	if (!handle.HasHandle()) {
		STREAM_LOG_WARNING("World::EntityStorage | entity handle has no handle.");
		return;
	}

	unregister_.emplace(handle);
	STREAM_LOG_DEBUG("World::EntityStorage | entity unregistered. handle: {}", handle.GetHandle());
}

void EntityStorage::Destroy() {
	while (!unregister_.empty()) {

		//!< 解放されるhandleを取得し、allocatorに返却する. コンテナからも削除する.
		const EntityHandle& handle = unregister_.front();
		STREAM_ASSERT(storage_.contains(handle), "entity handle not found.");
		storage_.erase(handle);
		allocator_.Free(handle.GetHandle());

		unregister_.pop();
	}
}

RefPtr<EntityBehaviour> EntityStorage::GetEntity(EntityHandle handle) const {
	STREAM_ASSERT(handle.HasHandle(), "entity handle has no handle.");

	if (!storage_.contains(handle)) {
		STREAM_LOG_WARNING("World::EntityStorage | entity handle not found. handle: {}", handle.GetHandle());
		return nullptr;
	}
	
	return storage_.at(handle).get();
}

EntityStorage* EntityStorage::GetInstance() noexcept {
	static EntityStorage instance;
	return &instance;
}
