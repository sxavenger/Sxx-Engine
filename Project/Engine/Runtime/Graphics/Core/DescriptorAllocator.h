#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "Device.h"
#include "Descriptor.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/CXXAttribute.hpp>
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Container/IndexAllocator.h>

//* c++
#include <queue>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorAllocator class
////////////////////////////////////////////////////////////////////////////////////////////
class DescriptorAllocator final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	DescriptorAllocator() noexcept = default;
	~DescriptorAllocator();

	//* allocator option *//

	void Init(
		const Device& device,
		DescriptorCategory category, UINT capacity
	);

	NODISCARD Descriptor Allocate();

	void Free(Descriptor::Handle&& handle);

	RefPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const { return descriptorHeap_.Get(); }

	uint32_t GetCapacity() const { return allocator_.GetCapacity(); }

	uint32_t GetUsedCount() const { return allocator_.GetUsedCount(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	SIZE_T handleOffset_ = 0; //!< デスクリプタヒープ内の割り当てるハンドルのオフセット.

	//* allocator *//

	IndexAllocator<UINT> allocator_;

	//* parameter *//

	DescriptorCategory category_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* category helper method *//

	static bool CheckShaderVisible(DescriptorCategory category);

	static D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorHeapType(DescriptorCategory category);

	//* initalize helper method *//

	static ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		RefPtr<ID3D12Device> device, DescriptorCategory category, UINT capacity
	);

	//* descriptor helper method *//

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(UINT index) const;

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(UINT index) const;

};

SXAVENGER_ENGINE_NAMESPACE_END
