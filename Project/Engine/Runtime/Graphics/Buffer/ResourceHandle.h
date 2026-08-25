#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "Resource.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class ResourceAllocator;

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceHandle class
////////////////////////////////////////////////////////////////////////////////////////////
class ResourceHandle final {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------
	using Type = uint64_t;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Handle structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Handle {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		Handle() noexcept = default;
		Handle(Type index) noexcept : index(index) {}

		//* handle option *//

		void Reset();

		//! @brief デスクリプタが有効かどうかを取得する.
		bool HasHandle() const;

		//! @brief デスクリプタのインデックスを取得する.
		//! @throw デスクリプタが無効な場合.
		Type GetIndex() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::optional<Type> index = std::nullopt;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ResourceHandle() noexcept = default;
	ResourceHandle(ResourceAllocator* allocator, const Handle& handle) : allocator_(allocator), handle_(handle) {}

	~ResourceHandle();

	//* handle option *//

	void Reset();

	//! @brief デスクリプタが有効かどうかを取得する.
	bool HasHandle() const { return handle_.HasHandle(); }

	//! @brief デスクリプタのインデックスを取得する.
	Type GetIndex() const { return handle_.GetIndex(); }

	//! @brief デスクリプタのハンドルを取得する.
	const Handle& GetHandle() const { return handle_; }

	//* resource option *//

	void SetName(const std::wstring_view& name) const;
	void SetName(const std::string_view& name) const;

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const;

	uint64_t GetCurrentIndex() const;

	Resource& GetResource();
	const Resource& GetResource() const;

	//* operator [copy] <ResourceHandle> (delete) *//

	ResourceHandle(const ResourceHandle&)            = delete;
	ResourceHandle& operator=(const ResourceHandle&) = delete;

	//* operator [move] <ResourceHandle> *//

	ResourceHandle(ResourceHandle&& other) noexcept;
	ResourceHandle& operator=(ResourceHandle&& other) noexcept;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* handle *//

	Handle handle_;

	//* allocator *//

	RefPtr<ResourceAllocator> allocator_ = nullptr;

};

SXAVENGER_ENGINE_NAMESPACE_END
