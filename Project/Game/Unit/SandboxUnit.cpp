#include "SandboxUnit.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine [graphics]
#include <Runtime/Graphics/Core.h>

//* engine [framework]
#include <Runtime/Framework/Core/Context.h>
#include <Runtime/Framework/Unit/WindowUnit.h>

//* engine [editor]
#include <Runtime/Editor/Slate/Renderer/SlateImGuiRenderer.h>

//* lib
#include <Lib/Format/Json/JsonFile.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SandboxUnit class methods
////////////////////////////////////////////////////////////////////////////////////////////

SandboxUnit::SandboxUnit() {
	//!< windowの追加
	Sxx::Framework::Context::Push<Sxx::Framework::WindowUnit>();
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

	RefPtr<Sxx::Framework::WindowUnit> unit
		= Sxx::Framework::Context::GetUnit<Sxx::Framework::WindowUnit>(); //!< windowを管理しているunitを取得.

	auto& viewport = unit->GetViewport();

	auto& buffer = viewport.GetCurrentBackBuffer();
	buffer.TransitionRenderTarget(context);
	buffer.ClearRenderTarget(context, Color4f::Convert(0x9BA8A8FF));
	buffer.TransitionPresent(context);
}
