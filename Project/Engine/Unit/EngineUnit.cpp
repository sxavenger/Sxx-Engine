#include "EngineUnit.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine [platform]
#include <Runtime/Platform/WinApp.h>
#include <Runtime/Platform/Input/Input.h>

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

void EngineUnit::Setup(Framework::Pipeline& pipeline) {

	pipeline.SetProcess(Framework::Phase::BeginFrame, Framework::Priority::Highest, [this]() {
		//!< フレームクロックの更新処理
		frameClock_.BeginFrame();

		Sxx::Platform::Input::Update(); //!< 入力の更新処理
		
		Sxx::Graphics::Core::CheckDeviceStatus(); //!< deviceの状態をチェックする.
		Sxx::Graphics::Core::IncrementFrame(); //!< frameを更新する.
	});

	pipeline.SetProcess(Framework::Phase::EndFrame, Framework::Priority::Highest, [this]() {
		//!< graphicsの終了frame処理
		Sxx::Graphics::Core::SubmitDirectQueueAdvance();

		Sxx::Graphics::Core::FreeDescriptor(); //!< descriptorの解放処理
		Sxx::Graphics::Core::FreeResource();   //!< resourceの解放処理
	});

	pipeline.SetProcess(Framework::Phase::EndFrame, Framework::Priority::Lowest, [this]() {
		//!< フレームクロックの更新処理
		frameClock_.EndFrame();
	});

	pipeline.SetProcess(Framework::Phase::Terminate, Framework::Priority::Highest, [this]() {
		//!< Worldの終了処理.
		Sxx::World::EntityStorage::GetInstance()->Destroy();
		Sxx::World::ComponentStorage::GetInstance()->Destroy();

		//!< Schedulerの終了処理.
		Scheduler::System::Shutdown();

		//!< graphicsのQueueの処理の終了.
		Sxx::Graphics::Core::SubmitDirectQueueWait(); //!< direct queueの処理を全て実行する.
	});
}

void EngineUnit::InitEngine() {

	StreamLogger::Init();
	CrashHandler::Install();

	StreamLogger::Info("Sxavenger Engine >> version: {}", SXAVENGER_ENGINE_VERSION);

  configuration_.Load("Engine/Packages/config/Platform.toml");
	configuration_.Load("Engine/Packages/config/Graphics.toml");
	configuration_.Load("Engine/Packages/config/Application.toml");
	// TODO: engine関係のファイルがGameからの相対パス指定なので修正する

	Platform::WinApp::Init();
	Platform::Input::Init(configuration_);

	Graphics::Core::Init(configuration_);

	frameClock_.Init(configuration_);

	Scheduler::System::Init();
}

void EngineUnit::TermEngine() {

	Graphics::Core::Term();

	Platform::Input::Shutdown();

	Platform::WinApp::Term();

	CrashHandler::Uninstall();

}
