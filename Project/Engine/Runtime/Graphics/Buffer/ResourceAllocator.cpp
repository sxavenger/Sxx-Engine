#include "ResourceAllocator.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceHandle class methods
////////////////////////////////////////////////////////////////////////////////////////////

ResourceAllocator::~ResourceAllocator() {
	Free(); //!< 解放キューに残っているindexを解放する.

	if (allocator_.GetUsedCount() != 0) {
		STREAM_LOG_WARNING(
			"Graphics::ResourceAllocator | descriptor leak detected. used count: {}.", allocator_.GetUsedCount()
		);
		//!< デスクリプタリークの警告. DescriptorAllocatorが解放されるときに、使用中のDescriptorがある場合は警告を出す.
	}

	STREAM_LOG_INFO("Graphics::ResourceAllocator | terminate.");
}

ResourceHandle ResourceAllocator::Allocate(const Device& device, const ResourceDesc& desc, uint8_t count) {

	ResourceHandle::Handle handle;
	handle.index = allocator_.Allocate(); //!< indexの取得.

	//!< Bufferの作成.
	Buffer& buffer = pool_[handle.GetIndex()] = Buffer(count);
	for (uint8_t i = 0; i < count; ++i) {
		buffer[i] = Resource::CreateCommitted(device, desc);
	}

	STREAM_LOG_DEBUG(
		"Graphics::ResourceAllocator | allocate resource handle. index: {}, buffer count: {}", handle.GetIndex(), count
	);

	return ResourceHandle(this, handle);
}

void ResourceAllocator::Release(ResourceHandle::Handle&& handle) {
	if (!handle.HasHandle()) {
		STREAM_LOG_WARNING("Graphics::ResourceAllocator | release resource handle. handle is not valid.");
		return; //!< handleが無効な場合は警告を出して終了.
	}

	STREAM_LOG_DEBUG(
		"Graphics::ResourceAllocator | release resource handle. index: {}", handle.GetIndex()
	);

	freeQueue_.emplace(handle.GetIndex()); //!< 解放されたindexをキューに追加する.
	handle.Reset();
}

void ResourceAllocator::Free() {
	while (!freeQueue_.empty()) {
		ResourceHandle::Type index = freeQueue_.front();
		freeQueue_.pop();

		STREAM_LOG_DEBUG(
			"Graphics::ResourceAllocator | free resource handle. index: {}", index
		);

		allocator_.Free(index);
	}
}

ResourceAllocator::Buffer& ResourceAllocator::GetBuffer(const ResourceHandle::Handle& handle) {
	STREAM_ASSERT(handle.HasHandle(), "resource handle is not valid.");
	return pool_.at(handle.GetIndex());
}

const ResourceAllocator::Buffer& ResourceAllocator::GetBuffer(const ResourceHandle::Handle& handle) const {
	STREAM_ASSERT(handle.HasHandle(), "resource handle is not valid.");
	return pool_.at(handle.GetIndex());
}

Resource& ResourceAllocator::GetResource(const ResourceHandle::Handle& handle) {
	Buffer& buffer = GetBuffer(handle); //!< handleに対応するBufferを取得.

	uint64_t frameIndex = currentFrame_ % buffer.size(); //!< 現在のフレームに対応するリソースを取得.
	return buffer[frameIndex];
}

const Resource& ResourceAllocator::GetResource(const ResourceHandle::Handle& handle) const {
	const Buffer& buffer = GetBuffer(handle); //!< handleに対応するBufferを取得.

	uint64_t frameIndex = currentFrame_ % buffer.size(); //!< 現在のフレームに対応するリソースを取得.
	return buffer[frameIndex];
}
