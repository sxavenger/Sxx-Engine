#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Platform/Window.h>
#include <Runtime/Graphics/Buffer/SwapChain.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Application)

////////////////////////////////////////////////////////////////////////////////////////////
// Viewport class
////////////////////////////////////////////////////////////////////////////////////////////
class Viewport final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* viewport option *//

	void Create(
		const std::wstring& name, const Vector2ui& client,
		Platform::Window::Category category,
		DXGI_FORMAT format,
		FlagEnum<Platform::Window::Style> style = Platform::Window::Style::Default
	);

	void Update();

	//* window option *//

	void Open() const;

	void Close();

	Vector2ui GetClient() const;

	Platform::Window& GetWindow() { return window_; }
	const Platform::Window& GetWindow() const { return window_; }

	//* swap chain option *//

	Graphics::SwapChain::Buffer& GetCurrentBackBuffer();

	void Present(bool vsync);

	Graphics::SwapChain& GetSwapChain() { return swapChain_; }
	const Graphics::SwapChain& GetSwapChain() const { return swapChain_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Platform::Window window_;
	Graphics::SwapChain swapChain_;

};

SXAVENGER_ENGINE_NAMESPACE_END
