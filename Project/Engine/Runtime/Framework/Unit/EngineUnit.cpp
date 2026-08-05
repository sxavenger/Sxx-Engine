#include "EngineUnit.h"
SXAVENGER_ENGINE_USING_(Framework)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine [platform]
#include <Runtime/Platform/WinApp.h>

//* engine [graphics]
#include <Runtime/Graphics/Core.h>

//* engine [scheduler]
#include <Runtime/Scheduler/System.h>

//* engine [world]
#include <Runtime/World/Entity/EntityStorage.h>
#include <Runtime/World/Component/ComponentStorage.h>

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Logger/CrashHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EngineUnit class methods
////////////////////////////////////////////////////////////////////////////////////////////

EngineUnit::EngineUnit() {
	InitEngine(); //!< エンジンの初期化処理
}

EngineUnit::~EngineUnit() {
	TermEngine(); //!< エンジンの終了処理
}

void EngineUnit::Setup(Pipeline& pipeline) {

	pipeline.SetProcess(Phase::BeginFrame, Priority::Highest, [this]() {
		//!< フレームクロックの更新処理
		frameClock_.BeginFrame();

		//!< graphicsの開始frame処理
		Sxx::Graphics::Core::BeginFrame();
	});

	pipeline.SetProcess(Phase::EndFrame, Priority::Highest, [this]() {
		//!< graphicsの終了frame処理
		Sxx::Graphics::Core::SubmitDirectQueue();
	});

	pipeline.SetProcess(Phase::EndFrame, Priority::Lowest, [this]() {
		//!< フレームクロックの更新処理
		frameClock_.EndFrame();
	});

	pipeline.SetProcess(Phase::Terminate, Priority::Highest, [this]() {
		//!< Worldの終了処理.
		Sxx::World::EntityStorage::GetInstance()->Destroy();
		Sxx::World::ComponentStorage::GetInstance()->Destroy();

		//!< Schedulerの終了処理.
		Scheduler::System::Shutdown();

		//!< graphicsのQueueの処理の終了.
		Sxx::Graphics::Core::SubmitDirectQueue();
	});
}

void EngineUnit::InitEngine() {
	StreamLogger::Init();
	CrashHandler::Install();

	configuration_.Load("Engine/Packages/config/Graphics.toml");
	configuration_.Load("Engine/Packages/config/Application.toml");
	// TODO: engine関係のファイルがGameからの相対パス指定なので修正する

	Platform::WinApp::Init();

	input_.Init(Platform::InputSystem::Mode::Async);

	Graphics::Core::Init(configuration_);

	frameClock_.Init(configuration_);

	Scheduler::System::Init();
}

void EngineUnit::TermEngine() {

	Graphics::Core::Term();

	input_.Shutdown();

	Platform::WinApp::Term();

	CrashHandler::Uninstall();

}
