#include "InputContext.h"
SXAVENGER_ENGINE_USING_(Platform)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Format/Toml/TomlReader.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [InputContext] Settings structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void InputContext::Settings::Parse(const toml::table& config) {
	TomlReader<Mode>::Find(config, "mode", mode); //!< 入力の更新モードの取得.
}

InputContext::Settings InputContext::Settings::ParseFromConfig(const Configuration& config) {
	Settings settings;

	if (!config.Contains(kConfigPath.GetPath())) {
		StreamLogger::Warning(
			"Platform::InputContext::Settings | config does not exist. path: {}", kConfigPath.GetPath()
		);
		return settings; //!< 設定が存在しない.
	}

	settings.Parse(config.GetConfig(Settings::kConfigPath.GetPath()));        //!< 全体設定の取得.
	settings.Parse(config.GetConfig(Settings::kConfigPath.GetProfilePath())); //!< プロファイル設定の取得.

	return settings;
}

void InputContext::Settings::Log(const Settings& settings) {
	StreamLogger::Debug("Platform::InputContext::Settings | mode: {}", settings.mode);
}

////////////////////////////////////////////////////////////////////////////////////////////
// InputContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void InputContext::Init(const Configuration& config) {

	settings_ = Settings::ParseFromConfig(config); //!< 設定の取得.
	Settings::Log(settings_); //!< 設定のログ.

	HINSTANCE hinst = GetModuleHandle(nullptr);

	//!< DirectInputの初期化
	auto hr = DirectInput8Create(
		hinst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(directInput_.GetAddressOf()),
		nullptr
	);
	ComPtrUtil::Assert(hr, L"failed to create DirectInput.");

	//!< dinput関係の初期化

	keyboard_.Init(directInput_.Get());
	mouse_.Init(directInput_.Get());

	//!< xinput関係の初期化

	for (uint8_t i = 0; i < XUSER_MAX_COUNT; ++i) {
		gamepads_[i].Init(i);
	}

	//!< thread関係
	
	if (settings_.mode == Mode::Async) {
		//!< 非同期スレッドで更新する場合はスレッドを生成
		running_.store(true, std::memory_order::release); //!< running_をtrueに設定してスレッドを開始する.
		thread_ = std::thread([this]() {
			StreamLogger::Info("Platform::InputContext | begin async input thread.");

			while (running_.load(std::memory_order::acquire)) {
				UpdateInput(); //!< 入力の更新
				std::this_thread::sleep_for(std::chrono::milliseconds(1)); //!< CPU使用率を抑えるために少し待機
			}

			StreamLogger::Info("Platform::InputContext | end async input thread.");
		});
		SetThreadDescription(thread_.native_handle(), L"Platform::InputContext | Async Input Thread");
	}

	StreamLogger::Info(
		"Platform::InputContext | input context initialized. mode: {}", settings_.mode
	);
}

void InputContext::Shutdown() {
	running_.store(false, std::memory_order::release); //!< スレッドの終了の通知.

	if (thread_.joinable()) {
		StreamLogger::Debug("Platform::InputContext | waiting for input thread to terminate...");
		thread_.join(); //!< スレッドの終了を待つ
	}

	StreamLogger::Info("Platform::InputContext | input context shutdown.");
}

void InputContext::Update() {
	if (settings_.mode == Mode::Sync) {
		UpdateInput(); //!< メインスレッドで更新する場合は直接更新
	}

	CommitInput();
}

void InputContext::SetWindow(HWND hwnd) {
	//!< dinputのwindow設定
	keyboard_.SetWindow(hwnd);
	mouse_.SetWindow(hwnd);
}

void InputContext::UpdateInput() {
	//!< inputのstackの更新

	std::unique_lock lock(mutex_);

	//!< dinputの更新
	keyboard_.Update();
	mouse_.Update();

	//!< xinputの更新
	for (auto& gamepad : gamepads_) {
		gamepad.Update();
	}
}

void InputContext::CommitInput() {
	//!< stackのinputをcurrentに反映させる

	//!< dinputの更新
	keyboard_.CommitStackInput();
	mouse_.CommitStackInput();

	//!< xinputの更新
	for (auto& gamepad : gamepads_) {
		gamepad.CommitStackInput();
	}
}
