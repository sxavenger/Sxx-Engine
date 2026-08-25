#include "ResourceHandle.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "ResourceAllocator.h"

//* lib
#include <Lib/String/UnicodeConverter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [ResourceHandle] Handle structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ResourceHandle::Handle::Reset() {
	*this = Handle();
}

bool ResourceHandle::Handle::HasHandle() const {
	return index.has_value();
}

ResourceHandle::Type ResourceHandle::Handle::GetIndex() const {
	STREAM_ASSERT(index.has_value(), "resource handle is not valid.");
	return index.value();
}

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceHandle class methods
////////////////////////////////////////////////////////////////////////////////////////////

ResourceHandle::~ResourceHandle() {
	Reset();
}

void ResourceHandle::Reset() {
	if (handle_.HasHandle()) {
		allocator_->Release(std::move(handle_)); //!< allocatorに解放を通知.
	}

	allocator_ = nullptr;
	handle_.Reset();
}

void ResourceHandle::SetName(const std::wstring_view& name) const {
	STREAM_ASSERT(allocator_ != nullptr, "resource allocator is null.");

	if (!handle_.HasHandle()) {
		STREAM_LOG_WARNING(L"Graphics::ResourceHandle | resource handle is not valid. cannot set name. name: {}", name);
		return; //!< handleが無効な場合は設定できない.
	}

	const ResourceAllocator::Buffer& buffer = allocator_->GetBuffer(handle_); //!< allocatorからBufferを取得.
	for (uint8_t i = 0; i < buffer.size(); ++i) {
		buffer[i].SetName(std::format(L"{} | [{}]", name, i)); //!< Buffer内のResourceに名前を設定.
	}
}

void ResourceHandle::SetName(const std::string_view& name) const {
	SetName(UnicodeConverter::ConvertW(name));
}

D3D12_GPU_VIRTUAL_ADDRESS ResourceHandle::GetGpuVirtualAddress() const {
	return GetResource().GetGpuVirtualAddress(); //!< allocatorからResourceを取得してGPU仮想アドレスを返す.
}

uint64_t ResourceHandle::GetCurrentIndex() const {
	return allocator_->GetCurrentIndex(handle_); //!< allocatorから現在のindexを取得.
}

Resource& ResourceHandle::GetResource() {
	STREAM_ASSERT(allocator_ != nullptr, "resource allocator is null.");
	STREAM_ASSERT(handle_.HasHandle(), "resource handle is not valid.");
	return allocator_->GetResource(handle_); //!< allocatorからResourceを取得.
}

const Resource& ResourceHandle::GetResource() const {
	STREAM_ASSERT(allocator_ != nullptr, "resource allocator is null.");
	STREAM_ASSERT(handle_.HasHandle(), "resource handle is not valid.");
	return allocator_->GetResource(handle_); //!< allocatorからResourceを取得.
}

ResourceHandle::ResourceHandle(ResourceHandle&& other) noexcept
	: allocator_(std::exchange(other.allocator_, nullptr)), handle_(std::exchange(other.handle_, {})) {
}

ResourceHandle& ResourceHandle::operator=(ResourceHandle&& other) noexcept {
	//!< thisへmove.
	allocator_ = std::exchange(other.allocator_, nullptr);
	handle_    = std::exchange(other.handle_, {});

	return *this;
}
