#include "Descriptor.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "DescriptorAllocator.h"

//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [Descriptor] Handle structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Descriptor::Handle::Reset() {
	*this = Handle();
}

DescriptorCategory Descriptor::Handle::GetCategory() const {
	StreamLogger::Assert(category.has_value(), "descriptor handle is not valid.");
	return category.value();
}

UINT Descriptor::Handle::GetIndex() const {
	StreamLogger::Assert(category.has_value(), "descriptor handle is not valid.");
	return index;
}

const D3D12_CPU_DESCRIPTOR_HANDLE& Descriptor::Handle::GetCPUHandle() const {
	StreamLogger::Assert(category.has_value(), "descriptor handle is not valid.");
	return cpu;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& Descriptor::Handle::GetGPUHandle() const {
	StreamLogger::Assert(category.has_value(), "descriptor handle is not valid.");
	StreamLogger::Assert(gpu.has_value(), "descriptor handle is not valid.");
	return gpu.value();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Descriptor class methods
////////////////////////////////////////////////////////////////////////////////////////////

Descriptor::~Descriptor() {
	Reset();
}

void Descriptor::Reset() {
	if (handle_.HasHandle()) {
		allocator_->Free(std::move(handle_)); //!< デスクリプタの解放.
	}

	allocator_ = nullptr;
	handle_.Reset();
}

Descriptor::Descriptor(Descriptor&& other) noexcept {
	//!< thisへmove.
	allocator_ = other.allocator_;
	handle_    = std::move(other.handle_);

	//!< otherを初期化.
	other.allocator_ = nullptr;
	other.handle_.Reset();
}

Descriptor& Descriptor::operator=(Descriptor&& other) noexcept {
	//!< thisへmove.
	allocator_ = other.allocator_;
	handle_    = std::move(other.handle_);

	//!< otherを初期化.
	other.allocator_ = nullptr;
	other.handle_.Reset();

	return *this;
}
