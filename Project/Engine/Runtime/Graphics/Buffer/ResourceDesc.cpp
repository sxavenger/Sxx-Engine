#include "ResourceDesc.h"
SXAVENGER_ENGINE_USING_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

const D3D12_CLEAR_VALUE* ResourceDesc::GetClearValue() const {
	return clearValue.has_value() ? &clearValue.value() : nullptr;
}

ResourceDesc ResourceDesc::CreateBufferDesc(
	D3D12_HEAP_TYPE type,
	size_t size,
	D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state) {

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

	return ResourceDesc(prop, desc, state, std::nullopt);
}

ResourceDesc ResourceDesc::CreateTextureDesc(
	D3D12_RESOURCE_DIMENSION dimension,
	UINT width, UINT height, UINT16 depthOrArraySize, UINT16 miplevels,
	DXGI_FORMAT format,
	D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state,
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

	return ResourceDesc(prop, desc, state, clearValue);
}
