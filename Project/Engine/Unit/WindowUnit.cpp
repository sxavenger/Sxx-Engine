#include "WindowUnit.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
// WindowUnit class methods
////////////////////////////////////////////////////////////////////////////////////////////

void WindowUnit::Setup(Framework::Pipeline& pipeline) {
	pipeline.SetProcess(Framework::Phase::Initialize, Framework::Priority::Normal, [this]() { InitWindow(); });
	pipeline.SetProcess(Framework::Phase::BeginFrame, Framework::Priority::Highest, [this]() { UpdateWindow(); });
	pipeline.SetProcess(Framework::Phase::EndFrame, Framework::Priority::Normal, [this]() { PresentWindow(); });

	pipeline.SetCondition([this]() { return !Platform::Window::ProcessMessage(); });
}

void WindowUnit::InitWindow() {
	viewport_.Create(
		L"[Sxavenger Engine] WindowUnit", { 1280, 720 },
		Platform::Window::Category::Main,
		DXGI_FORMAT_R8G8B8A8_UNORM
	);
}

void WindowUnit::UpdateWindow() {
	viewport_.Update(); //!< XXX: SwapChainのリサイズはPresentの完了後に行う. (Present後にGPU同期しなければならない.)
}

void WindowUnit::PresentWindow() {
	viewport_.Present(false);
}
