#include "SandboxUnit.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine [graphics]
#include <Engine/Runtime/Graphics/Core.h>

//* engine [framework]
#include <Engine/Runtime/Framework/Core/Context.h>

//* engine [unit]
#include <Engine/Unit/WindowUnit.h>
//#include <Engine/Unit/SlateEditorUnit.h>

//* lib
#include <Lib/Format/Json/JsonFile.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SandboxUnit class methods
////////////////////////////////////////////////////////////////////////////////////////////

SandboxUnit::SandboxUnit() {
#ifdef DEVELOPMENT
	//Sxx::Framework::Context::Push<Sxx::SlateEditorUnit>();
#else
	//!< windowの追加
	Sxx::Framework::Context::Push<Sxx::WindowUnit>();
#endif
}

SandboxUnit::~SandboxUnit() {
}

void SandboxUnit::Setup(Sxx::Framework::Pipeline& pipeline) {
	pipeline.SetProcess(Sxx::Framework::Phase::Initialize, Sxx::Framework::Priority::Normal, [this]() { InitSandbox(); });
	pipeline.SetProcess(Sxx::Framework::Phase::Terminate, Sxx::Framework::Priority::Normal, [this]() { TermSandbox(); });
	pipeline.SetProcess(Sxx::Framework::Phase::Update, Sxx::Framework::Priority::Normal, [this]() { UpdateSandbox(); });
	pipeline.SetProcess(Sxx::Framework::Phase::Render, Sxx::Framework::Priority::Normal, [this]() { RenderSandbox(); });
}

void SandboxUnit::InitSandbox() {

}

void SandboxUnit::TermSandbox() {
}

void SandboxUnit::UpdateSandbox() {
}

void SandboxUnit::RenderSandbox() {

	auto& context = Sxx::Graphics::Core::GetCommandContextDirect();

	if (Sxx::Framework::Context::HasUnit<Sxx::WindowUnit>()) {

		RefPtr<Sxx::WindowUnit> unit
			= Sxx::Framework::Context::GetUnit<Sxx::WindowUnit>(); //!< windowを管理しているunitを取得.

		auto& viewport = unit->GetViewport();

		auto& buffer = viewport.GetCurrentBackBuffer();
		buffer.TransitionRenderTarget(context);
		buffer.ClearRenderTarget(context, Color4f::Convert(0x9BA8A8FF));
		buffer.TransitionPresent(context);
	}
}
