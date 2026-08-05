#include "Resource.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/String/UnicodeConverter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Resource class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Resource::Reset() {
	if (resource_ != nullptr) {
		resource_.Reset();
	}

	current_ = D3D12_RESOURCE_STATE_COMMON;
}

void Resource::SetName(const std::wstring_view& name) const {
	if (resource_ == nullptr) {
		StreamLogger::Warning(L"Graphics::Resource | resource is null. cannot set name. name: {}", name);
		return; //!< resourceがnullptrの場合は設定できない.
	}

	resource_->SetName(name.data());
}

void Resource::SetName(const std::string_view& name) const {
	if (resource_ == nullptr) {
		StreamLogger::Warning("Graphics::Resource | resource is null. cannot set name. name: {}", name);
		return; //!< resourceがnullptrの場合は設定できない.
	}

	SetName(UnicodeConverter::ConvertW(name));
}

void Resource::Map(void** data, const std::optional<D3D12_RANGE>& range) {
	StreamLogger::Assert(resource_ != nullptr, "resource is null.");
	resource_->Map(0, range.has_value() ? &range.value() : nullptr, data);
}

void Resource::Unmap(const std::optional<D3D12_RANGE>& range) {
	StreamLogger::Assert(resource_ != nullptr, "resource is null.");
	resource_->Unmap(0, range.has_value() ? &range.value() : nullptr);
}

D3D12_GPU_VIRTUAL_ADDRESS Resource::GetGpuVirtualAddress() const {
	StreamLogger::Assert(resource_ != nullptr, "resource is null.");
	return resource_->GetGPUVirtualAddress();
}

D3D12_RESOURCE_DESC Resource::GetDesc() const {
	StreamLogger::Assert(resource_ != nullptr, "resource is null.");
	return resource_->GetDesc();
}

std::optional<D3D12_RESOURCE_BARRIER> Resource::CreateTransitionBarrier(D3D12_RESOURCE_STATES state) {
	if (current_ == state) {
		return std::nullopt; //!< すでに同じ状態の場合は遷移しない.
	}

	//!< barrierの生成
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = current_;
	barrier.Transition.StateAfter  = state;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	//!< 現在のstateの更新
	current_ = state;

	return barrier;
}

void Resource::Transition(const GraphicsCommandContext& context, D3D12_RESOURCE_STATES state) {

	std::optional<D3D12_RESOURCE_BARRIER> barrier = CreateTransitionBarrier(state);

	if (!barrier.has_value()) {
		return; //!< すでに同じ状態の場合は遷移しない.
	}

	context.GetCommandList()->ResourceBarrier(1, &barrier.value());
}

void Resource::TransitionExplicit(const GraphicsCommandContext& context, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {

	//!< barrierの生成
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter  = after;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	context.GetCommandList()->ResourceBarrier(1, &barrier);

	//!< 現在のstateの更新
	current_ = after;
}

void Resource::AppendTransitionBarrier(std::vector<D3D12_RESOURCE_BARRIER>& barriers, Resource& resource, D3D12_RESOURCE_STATES state) {
	std::optional<D3D12_RESOURCE_BARRIER> barrier = resource.CreateTransitionBarrier(state);

	if (!barrier.has_value()) {
		return; //!< すでに同じ状態の場合は遷移しない.
	}

	barriers.emplace_back(barrier.value());
}

Resource Resource::Wrap(ComPtr<ID3D12Resource>&& resource, D3D12_RESOURCE_STATES state) {
	Resource res = {};
	res.resource_ = std::move(resource);
	res.current_  = state;

	return res;
}

Resource Resource::CreateCommitted(
	const Device& device,
	const D3D12_HEAP_PROPERTIES& prop, const D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES state,
	const std::optional<D3D12_CLEAR_VALUE>& clearValue) {

	ComPtr<ID3D12Resource> resource;

	//!< resourceの作成
	auto hr = device.GetDevice()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		state,
		clearValue.has_value() ? &clearValue.value() : nullptr,
		IID_PPV_ARGS(resource.GetAddressOf())
	);
	ComPtrUtil::Assert(hr, L"resource create failed.");

	return Resource::Wrap(std::move(resource), state);

}

Resource Resource::CreateDimensionBuffer(
	const Device& device,
	D3D12_HEAP_TYPE type,
	size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state) {

	//!< 制約
	StreamLogger::Assert(size != 0, "buffer size is zero.");

	//!< propの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = type;

	//!< descの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Flags            = flags;
	desc.Width            = size;
	desc.Height           = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//!< resourceの生成
	return Resource::CreateCommitted(device, prop, desc, state, std::nullopt);
}

Resource Resource::CreateTexture(
	const Device& device,
	D3D12_RESOURCE_DIMENSION dimension,
	UINT width, UINT height, UINT16 depthOrArraySize, UINT16 miplevels,
	DXGI_FORMAT format,
	D3D12_RESOURCE_FLAGS flags,
	D3D12_RESOURCE_STATES state,
	const std::optional<D3D12_CLEAR_VALUE>& clearValue) {

	//!< propの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	//!< descの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = dimension;
	desc.Flags            = flags;
	desc.Width            = width;
	desc.Height           = height;
	desc.DepthOrArraySize = depthOrArraySize;
	desc.MipLevels        = miplevels;
	desc.Format           = format;
	desc.SampleDesc.Count = 1;

	return Resource::CreateCommitted(device, prop, desc, state, clearValue);
}
