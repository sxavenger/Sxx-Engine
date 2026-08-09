#include "SandboxUnit.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine [graphics]
#include <Engine/Runtime/Graphics/Core.h>

//* engine [framework]
#include <Engine/Runtime/Framework/Core/Context.h>

//* engine [assets]
#include <Engine/Runtime/Assets/Texture/Texture.h>
#include <Engine/Runtime/Assets/Handle/AssetHandle.h>
#include <Engine/Runtime/Assets/Importer/TextureImporter.h>

//* engine [rendering]
#include <Engine/Runtime/Rendering/Cache/TextureCache.h>

//* engine [unit]
#include <Engine/Unit/WindowUnit.h>

//* lib
#include <Lib/Format/Json/JsonFile.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SandboxUnit class methods
////////////////////////////////////////////////////////////////////////////////////////////

SandboxUnit::SandboxUnit() {
#ifdef DEVELOPMENT
	// TODO: EditorのUnitを追加する
	Sxx::Framework::Context::Push<Sxx::WindowUnit>(); //!< 仮で通常windowを追加する.
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
	Sxx::Assets::AssetHandle<Sxx::Assets::Texture> handle
		= Sxx::Assets::TextureImporter::Import("Engine/Packages/textures/common/uvchecker.asset");

	Sxx::Rendering::TextureCache cache;

	if (cache.GetAddress() != handle.Get()->GetAddress()) {
		cache.Cache(handle.WaitGet());
	}
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
