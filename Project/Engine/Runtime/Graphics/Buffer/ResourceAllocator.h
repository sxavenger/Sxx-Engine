#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Core/Device.h"
#include "ResourceDesc.h"
#include "Resource.h"
#include "ResourceHandle.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Container/IndexAllocator.h>

//* c++
#include <unordered_map>
#include <vector>
#include <limits>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceAllocator class
////////////////////////////////////////////////////////////////////////////////////////////
class ResourceAllocator final {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Buffer = std::vector<Resource>;
	using Pool   = std::unordered_map<ResourceHandle::Type, Buffer>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ResourceAllocator() noexcept = default;
	~ResourceAllocator();

	//* allocator option *//

	ResourceHandle Allocate(const Device& device, const ResourceDesc& desc, uint8_t count);

	void Release(ResourceHandle::Handle&& handle);

	void Free();

	void IncrementFrame() { currentFrame_++; }

	Buffer& GetBuffer(const ResourceHandle::Handle& handle);
	const Buffer& GetBuffer(const ResourceHandle::Handle& handle) const;

	uint64_t GetCurrentIndex(const ResourceHandle::Handle& handle) const;

	Resource& GetResource(const ResourceHandle::Handle& handle);
	const Resource& GetResource(const ResourceHandle::Handle& handle) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Pool pool_;

	//* allocator *//

	IndexAllocator<ResourceHandle::Type> allocator_ = IndexAllocator<ResourceHandle::Type>(std::numeric_limits<ResourceHandle::Type>::max()); //!< indexの最大数を設定.
	std::queue<ResourceHandle::Type> freeQueue_; //!< 解放されたindexのキュー.

	//* runtime *//

	uint64_t currentFrame_ = 0;

};

SXAVENGER_ENGINE_NAMESPACE_END
