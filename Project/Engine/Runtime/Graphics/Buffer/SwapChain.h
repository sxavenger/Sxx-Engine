#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Device/Device.h"
#include "../Descriptor/DescriptorHeaps.h"
#include "../Descriptor/Descriptor.h"
#include "../Command/GraphicsCommandContext.h"
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

		//* transition option *//

		void TransitionRenderTarget(const GraphicsCommandContext& context);

		void TransitionPresent(const GraphicsCommandContext& context);

		//* render target option *//

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
		const Vector2u& resolution, HWND hwnd
	);

	//! @brief スワップチェインのサイズを変更する.
	//! @pre GraphicsCommand側のコマンドリストは全て完了している必要がある.
	void Resize(
		const Device& device,
		const Vector2u& resolution, HWND hwnd
	);

	void Present(const Device& device, bool vsync);

	DXGI_FORMAT GetRenderTargetFormat() const;

	//* render pass option *//

	void BeginRenderPass(const GraphicsCommandContext& context, const Color4f& color);

	void EndRenderPass(const GraphicsCommandContext& context);

	//* buffer option *//

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

	static ComPtr<IDXGISwapChain4> CreateSwapChain(RefPtr<IDXGIFactory7> factory, RefPtr<ID3D12CommandQueue> queue, DXGI_FORMAT format, const Vector2u& resolution, HWND hwnd);

	static ComPtr<ID3D12Resource> GetBufferResource(uint32_t index, RefPtr<IDXGISwapChain4> swapChain);

};

SXAVENGER_ENGINE_NAMESPACE_END
