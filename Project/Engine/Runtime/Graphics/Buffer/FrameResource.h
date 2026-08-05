#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Core/Device.h"
#include "../Core/GraphicsCommandContext.h"
#include "Resource.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// FrameResource class
////////////////////////////////////////////////////////////////////////////////////////////
class FrameResource final {
	// TODO: ダブルバッファリング用のResourceを管理するクラスを作成する.
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* resource create option *//

	static Resource CreateCommitted(
		const Device& device,
		const D3D12_HEAP_PROPERTIES& prop, const D3D12_RESOURCE_DESC& desc,
		D3D12_RESOURCE_STATES state,
		const std::optional<D3D12_CLEAR_VALUE>& clearValue = std::nullopt
	);

	static Resource CreateDimensionBuffer(
		const Device& device,
		D3D12_HEAP_TYPE type,
		size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state
	);

	static Resource CreateTexture(
		const Device& device,
		D3D12_RESOURCE_DIMENSION dimension,
		UINT width, UINT height, UINT16 depthOrArraySize, UINT16 miplevels,
		DXGI_FORMAT format,
		D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state,
		const std::optional<D3D12_CLEAR_VALUE>& clearValue = std::nullopt
	);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<Resource, kFrameCount> resources_;

};

SXAVENGER_ENGINE_NAMESPACE_END
