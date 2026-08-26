#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Core/Device.h"
#include "Resource.h"
#include "ResourceAllocator.h"
#include "ResourceHandle.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseDimensionBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Category enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Category {
		Upload,
		Unordered,
		Readback
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	BaseDimensionBuffer(size_t stride) : stride_(stride) {}
	~BaseDimensionBuffer() = default;

	//* resource option *//

	//! @brief Resourceを解放する.
	void Reset();

	//! @brief Resourceのデバッグ用の名前を設定する.
	void SetName(const std::wstring_view& name) const;

	//! @brief Resourceのデバッグ用の名前を設定する.
	void SetName(const std::string_view& name) const;

	//! @brief ResourceのGPU仮想アドレスを取得する.
	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const;

	//! @brief Resourceのハンドルを取得する.
	const ResourceHandle::Handle& GetHandle() const { return handle_.GetHandle(); }

	//! @brief Resourceを取得する.
	Resource& GetResource() { return handle_.GetResource(); }

	//! @brief Resourceを取得する.
	const Resource& GetResource() const { return handle_.GetResource(); }

	//* parameter option *//

	//! @brief バッファの容量を取得する.
	uint32_t GetCapacity() const { return capacity_; }

	//! @brief バッファのstrideを取得する.
	size_t GetStride() const { return stride_; }

	//! @brief バッファのバイトサイズを取得する.
	size_t GetByteSize() const { return capacity_ * stride_; }

	//* operator [copy] <BaseDimensionBuffer> (delete) *//

	BaseDimensionBuffer(const BaseDimensionBuffer&)            = delete;
	BaseDimensionBuffer& operator=(const BaseDimensionBuffer&) = delete;

	//* operator [move] <BaseDimensionBuffer> *//

	BaseDimensionBuffer(BaseDimensionBuffer&& other) noexcept;
	BaseDimensionBuffer& operator=(BaseDimensionBuffer&& other) noexcept;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* Graphics *//

	ResourceHandle handle_;

	//* parameter *//

	uint32_t capacity_ = NULL;
	size_t stride_     = NULL;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* static methods *//

	static D3D12_HEAP_TYPE GetCategoryHeap(Category category);

	static D3D12_RESOURCE_STATES GetCategoryState(Category category);

	static D3D12_RESOURCE_FLAGS GetCategoryFlags(Category category);

	static void CreateBuffer(
		BaseDimensionBuffer& buffer,
		const Device& device, ResourceAllocator& allocator,
		Category category,
		uint32_t capacity, uint8_t frameCount
	);

};

SXAVENGER_ENGINE_NAMESPACE_END
