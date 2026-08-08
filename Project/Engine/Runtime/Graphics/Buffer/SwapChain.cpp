#include "SwapChain.h"
SXAVENGER_ENGINE_USING_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// [SwapChain] Buffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SwapChain::Buffer::TransitionRenderTarget(const GraphicsCommandContext& context) {
	resource.Transition(context, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void SwapChain::Buffer::TransitionPresent(const GraphicsCommandContext& context) {
	resource.Transition(context, D3D12_RESOURCE_STATE_PRESENT);
}

void SwapChain::Buffer::ClearRenderTarget(const GraphicsCommandContext& context, const Color4f& color) {
	TransitionRenderTarget(context); //!< ClearRenderTargetViewを呼ぶ前にRenderTarget状態に遷移させる.
	context.GetCommandList()->ClearRenderTargetView(
		descriptorRTV.GetCPUHandle(),
		color.Ptr(),
		0, nullptr
	);
}

void SwapChain::Buffer::OMSetRenderTarget(const GraphicsCommandContext& context) {
	TransitionRenderTarget(context); //!< OMSetRenderTargetsを呼ぶ前にRenderTarget状態に遷移させる.
	context.GetCommandList()->OMSetRenderTargets(
		1, &descriptorRTV.GetCPUHandle(), false,
		nullptr
	);
}

////////////////////////////////////////////////////////////////////////////////////////////
// SwapChain class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SwapChain::Init(
	const Device& device, DescriptorHeaps& descriptorHeaps, const GraphicsCommandContext& command,
	DXGI_FORMAT format,
	const Vector2ui& resolution, HWND hwnd) {

	//!< 設定の保存.
	format_ = format;

	//!< DirectXの初期化.
	swapChain_ = SwapChain::CreateSwapChain(device.GetFactory(), command.GetCommandQueue(), format, resolution, hwnd);

	//!< バッファの初期化.
	for (uint32_t i = 0; i < kFrameCount; ++i) {

		Buffer& buffer = buffers_[i];

		//!< resourceの取得
		buffer.resource
			= Resource::Wrap(SwapChain::GetBufferResource(i, swapChain_.Get()), D3D12_RESOURCE_STATE_PRESENT);

		buffer.resource.SetName(std::format(L"SwapChain Buffer (hwnd: {:p}) [{}] ", reinterpret_cast<const void*>(hwnd), i)); //!< resourceの名前を設定.

		//!< Descriptorの取得
		buffer.descriptorRTV = descriptorHeaps.Allocate(DescriptorCategory::RTV);

		//!< RenderTargetの作成
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format        = Graphics::ConvertToSRGBFormat(format); //!< SRGBに変換しておく.
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		device.GetDevice()->CreateRenderTargetView(
			buffer.resource.Get(),
			&desc,
			buffer.descriptorRTV.GetCPUHandle()
		);
	}

	StreamLogger::Info("Graphics::SwapChain | successfully initialized swap chain.");
}

void SwapChain::Resize(
	const Device& device,
	const Vector2ui& resolution, HWND hwnd) {

	if (swapChain_ == nullptr) {
		StreamLogger::Warning("Graphics::SwapChain | resize called but swap chain is nullptr.");
		return; //!< swapChainがnullptrの場合は何もしない.
	}

	for (uint32_t i = 0; i < kFrameCount; ++i) {
		Buffer& buffer = buffers_[i];
		buffer.resource.Reset(); //!< resourceの解放.
	}

	//!< swapChainのリサイズ
	swapChain_->ResizeBuffers(
		kFrameCount,
		resolution.x, resolution.y,
		format_, //!< formatは既存のものを使用する.
		DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
	);

	for (uint32_t i = 0; i < kFrameCount; ++i) {

		Buffer& buffer = buffers_[i];

		//!< resourceの取得
		buffer.resource
			= Resource::Wrap(SwapChain::GetBufferResource(i, swapChain_.Get()), D3D12_RESOURCE_STATE_PRESENT);

		buffer.resource.SetName(std::format(L"SwapChain Buffer (hwnd: {:p}) [{}] ", reinterpret_cast<const void*>(hwnd), i)); //!< resourceの名前を設定.

		// descriptorは既に確保されている.
		StreamLogger::Assert(buffer.descriptorRTV.HasHandle(), "Render Target Descriptor is not allocated.");

		//!< RenderTargetの作成
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format        = Graphics::ConvertToSRGBFormat(format_); //!< SRGBに変換しておく.
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		device.GetDevice()->CreateRenderTargetView(
			buffer.resource.Get(),
			&desc,
			buffer.descriptorRTV.GetCPUHandle()
		);
	}
}

void SwapChain::Present(const Device& device, bool vsync) {

	UINT syncInterval = 0;
	UINT flags        = 0;

	if (!device.GetSettings().allowTearing || !device.GetSupport().tearing) {
		vsync = true; //!< tearingが許可されていない場合は強制的にvsyncを有効にする.
	}

	if (vsync) {
		syncInterval = 1; //!< vsync有効の場合は1フレーム待つ.

	} else {
		flags |= DXGI_PRESENT_ALLOW_TEARING; //!< tearingが許可されている場合はフラグを設定する.
	}

	swapChain_->Present(syncInterval, flags);
}

SwapChain::Buffer& SwapChain::GetCurrentBackBuffer() {
	UINT index = swapChain_->GetCurrentBackBufferIndex();
	return buffers_[index];
}

ComPtr<IDXGISwapChain4> SwapChain::CreateSwapChain(RefPtr<IDXGIFactory7> factory, RefPtr<ID3D12CommandQueue> queue, DXGI_FORMAT format, const Vector2ui& resolution, HWND hwnd) {

	ComPtr<IDXGISwapChain4> swapChain;

	//!< descの設定
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width            = resolution.x;
	desc.Height           = resolution.y;
	desc.Format           = Graphics::ConvertToLinearFormat(format);
	desc.SampleDesc.Count = 1;
	desc.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount      = kFrameCount;
	desc.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Flags            = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	desc.Scaling          = DXGI_SCALING_STRETCH;

	auto hr = factory->CreateSwapChainForHwnd(
		queue,
		hwnd,
		&desc,
		nullptr,
		nullptr,
		reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf())
	);
	ComPtrUtil::Assert(hr, L"swap chain create failed.");

	return swapChain;
}

ComPtr<ID3D12Resource> SwapChain::GetBufferResource(uint32_t index, RefPtr<IDXGISwapChain4> swapChain) {

	ComPtr<ID3D12Resource> resource;

	auto hr = swapChain->GetBuffer(index, IID_PPV_ARGS(resource.GetAddressOf()));
	ComPtrUtil::Assert(hr, L"swap chain get buffer failed.");
	
	return resource;
}
