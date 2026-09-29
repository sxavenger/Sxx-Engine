#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cahce
#include "../Dynamic/BaseDynamicCache.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <cstdint>
#include <unordered_map>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// DynamicCacheCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class DynamicCacheCollection final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* collection option *//

	template <DynamicCache T, typename... Args>
	void Cache(uintptr_t address, const Args&... args);

	template <DynamicCache T>
	std::shared_ptr<T> GetCache(uintptr_t address) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<uintptr_t, std::shared_ptr<BaseDynamicCache>> collection_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	template <DynamicCache T>
	static std::shared_ptr<T> Cast(const std::shared_ptr<BaseDynamicCache>& pointer);

};

////////////////////////////////////////////////////////////////////////////////////////////
// DynamicCacheCollection class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <DynamicCache T, typename... Args>
void DynamicCacheCollection::Cache(uintptr_t address, const Args&... args) {

	if (!collection_.contains(address)) {
		//!< cacheが存在しない場合, 新しいinstanceを作成する
		collection_[address] = std::make_shared<T>();
	}

	std::shared_ptr<T> pointer = DynamicCacheCollection::Cast<T>(collection_[address]);
	pointer->Cache(args...);

}

template <DynamicCache T>
std::shared_ptr<T> DynamicCacheCollection::GetCache(uintptr_t address) const {
	STREAM_ASSERT(collection_.contains(address), "dynamic cache not found. address: {:x}", address);

	return DynamicCacheCollection::Cast<T>(collection_.at(address));
}

template <DynamicCache T>
inline std::shared_ptr<T> DynamicCacheCollection::Cast(const std::shared_ptr<BaseDynamicCache>& pointer) {
	std::shared_ptr<T> casted = std::dynamic_pointer_cast<T>(pointer);
	STREAM_ASSERT(casted != nullptr, "dynamic cache type mismatch.");

	return casted;
}

SXAVENGER_ENGINE_NAMESPACE_END
