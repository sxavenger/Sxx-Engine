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

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceDesc structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ResourceDesc final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* desc methods *//

	const D3D12_CLEAR_VALUE* GetClearValue() const;
	
	//* desc option *//

	static ResourceDesc CreateBufferDesc(
		D3D12_HEAP_TYPE type,
		size_t size,
		D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state
	);

	static ResourceDesc CreateTextureDesc(
		D3D12_RESOURCE_DIMENSION dimension,
		UINT width, UINT height, UINT16 depthOrArraySize, UINT16 miplevels,
		DXGI_FORMAT format,
		D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state,
		const std::optional<D3D12_CLEAR_VALUE>& clearValue = std::nullopt
	);

	//=========================================================================================
	// public variables
	//=========================================================================================

	D3D12_HEAP_PROPERTIES prop  = {};
	D3D12_RESOURCE_DESC desc    = {};
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

	std::optional<D3D12_CLEAR_VALUE> clearValue = std::nullopt;

};

SXAVENGER_ENGINE_NAMESPACE_END
