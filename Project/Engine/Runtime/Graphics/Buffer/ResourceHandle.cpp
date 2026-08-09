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
	StreamLogger::Assert(index.has_value(), "resource handle is not valid.");
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

void ResourceHandle::SetName(const std::wstring_view& name) {
	StreamLogger::Assert(allocator_ != nullptr, "resource allocator is null.");

	if (!handle_.HasHandle()) {
		StreamLogger::Warning(L"Graphics::ResourceHandle | resource handle is not valid. cannot set name. name: {}", name);
		return; //!< handleが無効な場合は設定できない.
	}

	ResourceAllocator::Buffer& buffer = allocator_->GetBuffer(handle_); //!< allocatorからBufferを取得.
	for (uint8_t i = 0; i < buffer.size(); ++i) {
		buffer[i].SetName(std::format(L"{} | [{}]", name, i)); //!< Buffer内のResourceに名前を設定.
	}
}

void ResourceHandle::SetName(const std::string_view& name) {
	SetName(UnicodeConverter::ConvertW(name));
}

Resource& ResourceHandle::GetResource() {
	StreamLogger::Assert(allocator_ != nullptr, "resource allocator is null.");
	StreamLogger::Assert(handle_.HasHandle(), "resource handle is not valid.");
	return allocator_->GetResource(handle_); //!< allocatorからResourceを取得.
}

ResourceHandle::ResourceHandle(ResourceHandle&& other) noexcept {
	//!< thisへmove.
	allocator_ = other.allocator_;
	handle_    = std::move(other.handle_);

	//!< otherを初期化.
	other.allocator_ = nullptr;
	other.handle_.Reset();
}

ResourceHandle& ResourceHandle::operator=(ResourceHandle&& other) noexcept {
	//!< thisへmove.
	allocator_ = other.allocator_;
	handle_    = std::move(other.handle_);

	//!< otherを初期化.
	other.allocator_ = nullptr;
	other.handle_.Reset();

	return *this;
}
