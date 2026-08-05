#include "Viewport.h"
SXAVENGER_ENGINE_USING_(Application)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Viewport class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Viewport::Create(
	const std::wstring& name, const Vector2ui& client,
	Platform::Window::Category category,
	DXGI_FORMAT format,
	FlagEnum<Platform::Window::Style> style) {

	//!< Platform::Windowの生成
	window_.Create(name, client, category, style);

	//!< Graphics::SwapChainの生成
	swapChain_.Init(
		Graphics::Core::GetDevice(), Graphics::Core::GetDescriptorHeaps(), Graphics::Core::GetCommandContextDirect(),
		format,
		client,
		window_.GetHwnd()
	);

	window_.Open(); //!< windowの表示
}

void Viewport::Update() {
	//!< windowのイベントを取得する
	Platform::Window::Event event = window_.GetEvent();

	if (event == Platform::Window::Event::Resize) {
		//!< windowのサイズが変更された場合、SwapChainのサイズを変更する
		Vector2ui client = window_.GetClient();
		swapChain_.Resize(Graphics::Core::GetDevice(), client, window_.GetHwnd());
	}
}

void Viewport::Open() const {
	window_.Open();
}

void Viewport::Close() {
	window_.Close();
}

Vector2ui Viewport::GetClient() const {
	return window_.GetClient();
}

Graphics::SwapChain::Buffer& Viewport::GetCurrentBackBuffer() {
	return swapChain_.GetCurrentBackBuffer();
}

void Viewport::Present(bool vsync) {
	swapChain_.Present(Graphics::Core::GetDevice(), vsync);
}
