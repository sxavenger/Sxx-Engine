#include "DescriptorAllocator.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/String/UnicodeConverter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorAllocator class methods
////////////////////////////////////////////////////////////////////////////////////////////

DescriptorAllocator::~DescriptorAllocator() {
	if (allocator_.GetUsedCount() != 0) {
		StreamLogger::Warning(
			"Graphics::DescriptorAllocator<{}> | descriptor leak detected. used count: {}.", category_, allocator_.GetUsedCount()
		);
		//!< デスクリプタリークの警告. DescriptorAllocatorが解放されるときに、使用中のDescriptorがある場合は警告を出す.
	}

	StreamLogger::Info(
		"Graphics::DescriptorAllocator<{}> | terminate.", category_
	);


}

void DescriptorAllocator::Init(
	const Device& device,
	DescriptorCategory category, UINT capacity) {

	//!< allocatorの初期化
	allocator_.Capacity(capacity);

	//!< descriptor heapの作成
	descriptorHeap_ = DescriptorAllocator::CreateDescriptorHeap(device.GetDevice(), category, capacity);
	handleOffset_   = device.GetDevice()->GetDescriptorHandleIncrementSize(DescriptorAllocator::GetDescriptorHeapType(category));
	
	//!< parameterの保存
	category_ = category;

	descriptorHeap_->SetName(
		std::format(L"DescriptorAllocator<{}>", UnicodeConverter::ConvertW(EnumUtil<DescriptorCategory>::GetName(category))).c_str()
	); //!< デバック用にheapに名前をつける. (ex."DescriptorAllocator<RTV>")

	StreamLogger::Info(
		"Graphics::DescriptorAllocator<{}> | initialization complete. capacity: {}", category, capacity
	);
}

Descriptor DescriptorAllocator::Allocate() {

	Descriptor::Handle handle(category_);

	handle.index = allocator_.Allocate(); //!< indexの取得.
	handle.cpu   = GetCPUDescriptorHandle(handle.index);

	if (DescriptorAllocator::CheckShaderVisible(category_)) {
		//!< ShaderVisibleなカテゴリはGPUハンドルも取得する.
		handle.gpu = GetGPUDescriptorHandle(handle.index);
	}

	StreamLogger::Debug(
		"Graphics::DescriptorAllocator<{}> | allocate descriptor handle. index: {},", category_, handle.index
	);

	return Descriptor(this, handle);
}

void DescriptorAllocator::Free(Descriptor::Handle&& handle) {
	allocator_.Free(handle.index); //!< indexの解放.

	StreamLogger::Debug(
		"Graphics::DescriptorAllocator<{}> | free descriptor handle. index: {},", category_, handle.index
	);

	handle.Reset();
}

bool DescriptorAllocator::CheckShaderVisible(DescriptorCategory type) {
	switch (type) {
		case DescriptorCategory::RTV:
		case DescriptorCategory::DSV:
			return false;

		case DescriptorCategory::SRV_CBV_UAV:
			return true;

		default:
			StreamLogger::Exception("category is not a valid value.");
	}
}

D3D12_DESCRIPTOR_HEAP_TYPE DescriptorAllocator::GetDescriptorHeapType(DescriptorCategory category) {
	switch (category) {
		case DescriptorCategory::RTV:
			return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		case DescriptorCategory::DSV:
			return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

		case DescriptorCategory::SRV_CBV_UAV:
			return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		default:
			StreamLogger::Exception("category is not a valid value.");
	}
}

ComPtr<ID3D12DescriptorHeap> DescriptorAllocator::CreateDescriptorHeap(RefPtr<ID3D12Device> device, DescriptorCategory category, UINT capacity) {

	ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type           = DescriptorAllocator::GetDescriptorHeapType(category);
	desc.NumDescriptors = capacity;
	desc.Flags          = DescriptorAllocator::CheckShaderVisible(category) //!< ShaderVisibleなカテゴリはShaderからアクセスできるようにする.
		? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	auto hr = device->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(descriptorHeap.GetAddressOf())
	);
	ComPtrUtil::Assert(hr, L"descriptor heap create failed.");

	return descriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::GetCPUDescriptorHandle(UINT index) const {
	D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += handleOffset_ * index;
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorAllocator::GetGPUDescriptorHandle(UINT index) const {
	StreamLogger::Assert(DescriptorAllocator::CheckShaderVisible(category_), "descriptor heap is not shader visible, so gpu handle is not available."); //!< ShaderVisibleでないカテゴリはGPUハンドルがない.
	D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += handleOffset_ * index;
	return handle;
}
