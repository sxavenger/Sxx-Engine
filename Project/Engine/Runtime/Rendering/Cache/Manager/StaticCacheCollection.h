#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cahce
#include "../Static/BaseStaticCache.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Uuid/Uuid.h>

//* c++
#include <cstdint>
#include <unordered_map>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// StaticCacheCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class StaticCacheCollection final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* collection option *//

	template <StaticCache T, typename... Args>
	void Cache(Uuid id, uintptr_t address, const Args&... args);

	template <StaticCache T>
	std::shared_ptr<T> GetCache(Uuid id) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<Uuid, std::shared_ptr<BaseStaticCache>> collection_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	template <StaticCache T>
	static std::shared_ptr<T> Cast(const std::shared_ptr<BaseStaticCache>& pointer);

};

////////////////////////////////////////////////////////////////////////////////////////////
// StaticCacheCollection class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <StaticCache T, typename... Args>
void StaticCacheCollection::Cache(Uuid id, uintptr_t address, const Args&... args) {

	if (!collection_.contains(id)) {
		//!< cacheが存在しない場合, 新しいinstanceを作成する
		collection_[id] = std::make_shared<T>();
	}

	{
		//!< cache更新の確認
		std::shared_ptr<BaseStaticCache> pointer = collection_[id];

		if (pointer->GetAddress() == address) {
			return; //!< cacheが最新の場合は更新しない
		}
	}

	std::shared_ptr<T> pointer = StaticCacheCollection::Cast<T>(collection_[id]);
	pointer->Cache(args...);
	
}

template <StaticCache T>
inline std::shared_ptr<T> StaticCacheCollection::GetCache(Uuid id) const {
	return StaticCacheCollection::Cast<T>(collection_.at(id));
}

template <StaticCache T>
std::shared_ptr<T> StaticCacheCollection::Cast(const std::shared_ptr<BaseStaticCache>& pointer) {
	std::shared_ptr<T> casted = std::dynamic_pointer_cast<T>(pointer);
	STREAM_ASSERT(casted != nullptr, "static cache type mismatch.");

	return casted;
}

SXAVENGER_ENGINE_NAMESPACE_END
