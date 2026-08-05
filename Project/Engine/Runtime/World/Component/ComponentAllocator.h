#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "BaseComponent.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/CXXAttribute.hpp>
#include <Lib/Container/IndexAllocator.h>

//* c++
#include <unordered_map>
#include <queue>
#include <numeric>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentAllocator class
////////////////////////////////////////////////////////////////////////////////////////////
class ComponentAllocator {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	//! @brief componentの格納コンテナ
	using Container = std::unordered_map<ComponentHandle, std::unique_ptr<BaseComponent>, ComponentHandle::Hash>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ComponentAllocator() noexcept : allocator_(std::numeric_limits<ComponentHandle::Type>::max()) {} //!< componentの取得可能数の設定.
	~ComponentAllocator() noexcept = default;

	//* component allocator option *//

	//! @brief componentのhandleを割り当てる
	NODISCARD ComponentHandle Allocate();

	//! @brief componentを登録する
	void Register(std::unique_ptr<BaseComponent>&& component);

	//! @brief componentを登録解除する
	void Unregister(ComponentHandle handle);

	//! @brief componentの破棄
	void Destroy();

	//! @brief componentを取得する
	RefPtr<BaseComponent> Get(ComponentHandle handle) const;

	size_t GetUsedCount() const { return allocator_.GetUsedCount(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* component container *//

	Container container_ = {};

	IndexAllocator<ComponentHandle::Type> allocator_;

	std::queue<ComponentHandle::Type> unregister_;

};

SXAVENGER_ENGINE_NAMESPACE_END
