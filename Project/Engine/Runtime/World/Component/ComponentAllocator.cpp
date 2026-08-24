#include "ComponentAllocator.h"
SXAVENGER_ENGINE_USING_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentAllocator class methods
////////////////////////////////////////////////////////////////////////////////////////////

ComponentHandle ComponentAllocator::Allocate() {
	return ComponentHandle(allocator_.Allocate());
}

void ComponentAllocator::Register(std::unique_ptr<BaseComponent>&& component) {
	container_.emplace(component->GetHandle(), std::move(component));
}

void ComponentAllocator::Unregister(ComponentHandle handle) {
	if (!handle.HasHandle()) {
		STREAM_LOG_ERROR("World::ComponentAllocator | unregister component handle has no handle.");
		return;
	}

	STREAM_ASSERT(container_.contains(handle), "component handle not found.");
	unregister_.emplace(handle.GetHandle());
}

void ComponentAllocator::Destroy() {
	while (!unregister_.empty()) {

		//!< 解放されるhandleを取得し、allocatorに返却する. コンテナからも削除する.
		const ComponentHandle& handle = ComponentHandle(unregister_.front());
		STREAM_ASSERT(container_.contains(handle), "component handle not found.");
		allocator_.Free(handle.GetHandle());
		container_.erase(handle);

		unregister_.pop();
	}
}

RefPtr<BaseComponent> ComponentAllocator::Get(ComponentHandle handle) const {
	STREAM_ASSERT(handle.HasHandle(), "component handle has no handle.");
	STREAM_ASSERT(container_.contains(handle), "component handle not found.");
	return container_.at(handle).get();
}
