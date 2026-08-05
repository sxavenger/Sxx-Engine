#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Core/Device.h"
#include "../Core/GraphicsCommandContext.h"
#include "../Core/DescriptorHeaps.h"
#include "../Core/Descriptor.h"
#include "Resource.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Vector2.h>
#include <Lib/Math/Color4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// SwapChain class
////////////////////////////////////////////////////////////////////////////////////////////
class SwapChain final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ColorSpace enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class ColorSpace : uint8_t {
		Rec709,
		// Rec2020_1000nit,
		// Rec2020_2000nit,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Buffer structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Buffer {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void TransitionRenderTarget(const GraphicsCommandContext& context);

		void TransitionPresent(const GraphicsCommandContext& context);

		void ClearRenderTarget(const GraphicsCommandContext& context, const Color4f& color);

		void OMSetRenderTarget(const GraphicsCommandContext& context);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Resource resource;
		Descriptor descriptorRTV;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	SwapChain() noexcept  = default;
	~SwapChain() noexcept = default;

	//* swap chain option *//

	void Init(
		const Device& device, DescriptorHeaps& descriptorHeaps, const GraphicsCommandContext& command,
		DXGI_FORMAT format,
		const Vector2ui& resolution, HWND hwnd
	);

	void Resize(
		const Device& device,
		const Vector2ui& resolution, HWND hwnd
	);

	void Present(const Device& device, bool vsync);

	Buffer& GetCurrentBackBuffer();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<IDXGISwapChain4> swapChain_;

	//* buffers *//

	std::array<Buffer, kFrameCount> buffers_;

	//* parameter *//

	DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;

	//* runtime parameter *//

	//ColorSpace colorSpace_ = ColorSpace::Rec709;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* initailize helper methods *//

	static ComPtr<IDXGISwapChain4> CreateSwapChain(RefPtr<IDXGIFactory7> factory, RefPtr<ID3D12CommandQueue> queue, DXGI_FORMAT format, const Vector2ui& resolution, HWND hwnd);

	static ComPtr<ID3D12Resource> GetBufferResource(uint32_t index, RefPtr<IDXGISwapChain4> swapChain);

};

SXAVENGER_ENGINE_NAMESPACE_END
