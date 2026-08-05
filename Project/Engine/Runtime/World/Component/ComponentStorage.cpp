#include "ComponentStorage.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

ComponentStorage::~ComponentStorage() noexcept {
	for (auto& [type, allocator] : storage_) {
		allocator.Destroy(); //!< componentの解放処理を行う.

		size_t used = allocator.GetUsedCount();

		if (used != 0) {
			StreamLogger::Warning("World::ComponentStorage<{}> | component handle reaked. count: {}", type.GetName(), allocator.GetUsedCount());
			//!< componentのhandleの解放がされていない場合は警告を出す.
		}
	}
}

void ComponentStorage::Unregister(const TypeInfo& type, ComponentHandle handle) {
	if (!handle.HasHandle()) {
		StreamLogger::Warning("World::ComponentStorage<{}> | component handle has no handle.", type.GetName());
		return;
	}

	StreamLogger::Debug("World::ComponentStorage<{}> | component unregistered. handle: {}", type.GetName(), handle.GetHandle());
	storage_[type].Unregister(handle);
}

void ComponentStorage::Destroy() {
	for (ComponentAllocator& allocator : storage_ | std::views::values) {
		allocator.Destroy();
	}
}

ComponentStorage* ComponentStorage::GetInstance() noexcept {
	static ComponentStorage instance;
	return &instance;
}
