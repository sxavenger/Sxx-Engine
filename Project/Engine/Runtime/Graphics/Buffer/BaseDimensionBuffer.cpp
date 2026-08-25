#include "BaseDimensionBuffer.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "ResourceDesc.h"

//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDimensionBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseDimensionBuffer::Reset() {
	handle_.Reset();
	capacity_ = NULL;
}

void BaseDimensionBuffer::SetName(const std::wstring_view& name) const {
	if (!handle_.HasHandle()) {
		STREAM_LOG_WARNING(L"Graphics::BaseDimensionBuffer | resource handle is not valid. cannot set name. name: {}", name);
		return; //!< handleが無効な場合は設定できない.
	}

	handle_.SetName(name); //!< ResourceHandleに名前を設定.
}

void BaseDimensionBuffer::SetName(const std::string_view& name) const {
	if (!handle_.HasHandle()) {
		STREAM_LOG_WARNING("Graphics::BaseDimensionBuffer | resource handle is not valid. cannot set name. name: {}", name);
		return; //!< handleが無効な場合は設定できない.
	}

	handle_.SetName(name); //!< ResourceHandleに名前を設定.
}

D3D12_GPU_VIRTUAL_ADDRESS BaseDimensionBuffer::GetGpuVirtualAddress() const {
	return handle_.GetGpuVirtualAddress();
}

BaseDimensionBuffer::BaseDimensionBuffer(BaseDimensionBuffer&& other) noexcept
	: handle_(std::move(other.handle_)), capacity_(std::exchange(other.capacity_, 0)), stride_(other.stride_) {
}

BaseDimensionBuffer& BaseDimensionBuffer::operator=(BaseDimensionBuffer&& other) noexcept {
	handle_   = std::move(other.handle_);
	capacity_ = std::exchange(other.capacity_, 0);
	stride_   = other.stride_;

	return *this;
}

D3D12_HEAP_TYPE BaseDimensionBuffer::GetCategoryHeap(Category category) {
	switch (category) {
		case Category::Upload:    return D3D12_HEAP_TYPE_UPLOAD;
		case Category::Unordered: return D3D12_HEAP_TYPE_DEFAULT;
		case Category::Readback:  return D3D12_HEAP_TYPE_READBACK;
		default: STREAM_EXCEPTION("invalid category.");
	}
}

D3D12_RESOURCE_STATES BaseDimensionBuffer::GetCategoryState(Category category) {
	switch (category) {
		case Category::Upload:    return D3D12_RESOURCE_STATE_GENERIC_READ;
		case Category::Unordered: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		case Category::Readback:  return D3D12_RESOURCE_STATE_COMMON;
		default: STREAM_EXCEPTION("invalid category.");
	}
}

D3D12_RESOURCE_FLAGS BaseDimensionBuffer::GetCategoryFlags(Category category) {
	switch (category) {
		case Category::Upload:    return D3D12_RESOURCE_FLAG_NONE;
		case Category::Unordered: return D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		case Category::Readback:  return D3D12_RESOURCE_FLAG_NONE;
		default: STREAM_EXCEPTION("invalid category.");
	}
}

void BaseDimensionBuffer::CreateBuffer(
	BaseDimensionBuffer& buffer,
	const Device& device, ResourceAllocator& allocator,
	Category category,
	uint32_t capacity, uint8_t frameCount) {

	//!< capacityの設定.
	buffer.capacity_ = capacity;

	//!< descの作成
	ResourceDesc desc = ResourceDesc::CreateBufferDesc(
		BaseDimensionBuffer::GetCategoryHeap(category),
		buffer.GetByteSize(),
		BaseDimensionBuffer::GetCategoryFlags(category),
		BaseDimensionBuffer::GetCategoryState(category)
	);

	//!< ResourceHandleの作成.
	buffer.handle_ = allocator.Allocate(
		device,
		desc,
		frameCount
	);
}
