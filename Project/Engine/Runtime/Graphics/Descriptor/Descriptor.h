#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"

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
class DescriptorAllocator;

////////////////////////////////////////////////////////////////////////////////////////////
// Descriptor class
////////////////////////////////////////////////////////////////////////////////////////////
class Descriptor final {
public:

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
		Handle(DescriptorCategory category) noexcept : category(category) {}

		//* handle option *//

		void Reset();

		//! @brief デスクリプタが有効かどうかを取得する.
		bool HasHandle() const { return category.has_value(); }

		//! @brief デスクリプタのカテゴリを取得する.
		//! @throw デスクリプタが無効な場合.
		DescriptorCategory GetCategory() const;

		//! @brief デスクリプタのインデックスを取得する.
		//! @throw デスクリプタが無効な場合.
		UINT GetIndex() const;

		//! @brief CPUハンドルを取得する.
		//! @throw デスクリプタが無効な場合.
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() const;

		//! @brief GPUハンドルを取得する.
		//! @throw デスクリプタが無効な場合、またはGPUハンドルを持っていない場合.
		const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* handle category *//

		std::optional<DescriptorCategory> category = std::nullopt;

		//* handle index *//

		UINT index                                     = NULL;
		D3D12_CPU_DESCRIPTOR_HANDLE cpu                = D3D12_CPU_DESCRIPTOR_HANDLE{};
		std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> gpu = std::nullopt;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//
		
	Descriptor() noexcept = default;
	Descriptor(DescriptorAllocator* allocator, const Handle& handle) : allocator_(allocator), handle_(handle) {}

	~Descriptor();

	//* descriptor option *//

	void Reset();

	//* handle option *//

	//! @brief デスクリプタが有効かどうかを取得する.
	bool HasHandle() const { return handle_.HasHandle(); }

	//! @brief デスクリプタのカテゴリを取得する.
	DescriptorCategory GetCategory() const { return handle_.GetCategory(); }

	//! @brief デスクリプタのインデックスを取得する.
	UINT GetIndex() const { return handle_.GetIndex(); }

	//! @brief CPUハンドルを取得する.
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() const { return handle_.GetCPUHandle(); }

	//! @brief GPUハンドルを取得する.
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const { return handle_.GetGPUHandle(); }

	//! @brief デスクリプタのハンドルを取得する.
	const Handle& GetHandle() const { return handle_; }

	//* operator [copy] <Descriptor> (delete) *//

	Descriptor(const Descriptor&)            = delete;
	Descriptor& operator=(const Descriptor&) = delete;

	//* operator [move] <Descriptor> *//

	Descriptor(Descriptor&& other) noexcept;
	Descriptor& operator=(Descriptor&& other) noexcept;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* allocator *//

	RefPtr<DescriptorAllocator> allocator_ = nullptr;

	//* handle parameter *//

	Handle handle_;

};

SXAVENGER_ENGINE_NAMESPACE_END
