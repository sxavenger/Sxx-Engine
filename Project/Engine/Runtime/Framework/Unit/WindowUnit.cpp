#include "WindowUnit.h"
SXAVENGER_ENGINE_USING_(Framework)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
// WindowUnit class methods
////////////////////////////////////////////////////////////////////////////////////////////

void WindowUnit::Setup(Pipeline& pipeline) {
	pipeline.SetProcess(Sxx::Framework::Phase::Initialize, Sxx::Framework::Priority::Normal, [this]() { InitWindow(); });
	pipeline.SetProcess(Sxx::Framework::Phase::BeginFrame, Sxx::Framework::Priority::Highest, [this]() { UpdateWindow(); });
	pipeline.SetProcess(Sxx::Framework::Phase::EndFrame, Sxx::Framework::Priority::Normal, [this]() { PresentWindow(); });

	pipeline.SetCondition([this]() { return !Sxx::Platform::Window::ProcessMessage(); });
}

void WindowUnit::InitWindow() {
	viewport_.Create(
		L"[Sxavenger Engine] WindowUnit", { 1280, 720 },
		Sxx::Platform::Window::Category::Main,
		DXGI_FORMAT_R8G8B8A8_UNORM
	);
}

void WindowUnit::UpdateWindow() {
	viewport_.Update(); //!< XXX: SwapChainのリサイズはPresentの完了後に行う. (Present後にGPU同期しなければならない.)
}

void WindowUnit::PresentWindow() {
	viewport_.Present(false);
}
