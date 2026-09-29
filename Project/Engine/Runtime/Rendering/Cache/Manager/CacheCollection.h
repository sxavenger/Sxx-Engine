#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cache
#include "DynamicCacheCollection.h"
#include "StaticCacheCollection.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// CacheCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class CacheCollection final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* dynamic cache option *//

	template <DynamicCache T, typename... Args>
	void Cache(uintptr_t address, const Args&... args);

	template <DynamicCache T>
	std::shared_ptr<T> GetCache(uintptr_t address) const;

	//* static cache option *//

	template <StaticCache T, typename... Args>
	void Cache(Uuid id, uintptr_t address, const Args&... args);

	template <StaticCache T>
	std::shared_ptr<T> GetCache(Uuid id) const;

	//* singleton *//

	static CacheCollection* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	DynamicCacheCollection dynamicCollection_; //!< 動的更新用のキャッシュのコレクション.
	StaticCacheCollection staticCollection_; //!< 静的更新用のキャッシュのコレクション.

};

////////////////////////////////////////////////////////////////////////////////////////////
// CacheCollection class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <DynamicCache T, typename... Args>
inline void CacheCollection::Cache(uintptr_t address, const Args&... args) {
	dynamicCollection_.Cache<T>(address, args...);
}

template <DynamicCache T>
inline std::shared_ptr<T> CacheCollection::GetCache(uintptr_t address) const {
	return dynamicCollection_.GetCache<T>(address);
}

template <StaticCache T, typename... Args>
inline void CacheCollection::Cache(Uuid id, uintptr_t address, const Args&... args) {
	staticCollection_.Cache<T>(id, address, args...);
}

template <StaticCache T>
inline std::shared_ptr<T> CacheCollection::GetCache(Uuid id) const {
	return staticCollection_.GetCache<T>(id);
}

SXAVENGER_ENGINE_NAMESPACE_END
