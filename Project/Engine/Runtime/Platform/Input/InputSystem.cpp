#include "InputSystem.h"
SXAVENGER_ENGINE_USING_(Platform)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/EnumUtil.h>

////////////////////////////////////////////////////////////////////////////////////////////
// InputSystem class methods
////////////////////////////////////////////////////////////////////////////////////////////

void InputSystem::Init(Mode mode) {

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
	
	if (mode == Mode::Async) {
		//!< 非同期スレッドで更新する場合はスレッドを生成
		thread_ = std::thread([this]() {
			StreamLogger::Info("Platform::InputSystem | begin async input thread.");

			while (!isTerminate_) {
				UpdateInput(); //!< 入力の更新
				std::this_thread::sleep_for(std::chrono::milliseconds(1)); //!< CPU使用率を抑えるために少し待機
			}

			StreamLogger::Info("Platform::InputSystem | end async input thread.");
		});
		SetThreadDescription(thread_.native_handle(), L"Platform::InputSystem | Async Input Thread");
	}

	mode_ = mode; //!< モードの保存

	StreamLogger::Info(
		"Platform::InputSystem | input system initialized. mode: {}", mode_
	);
}

void InputSystem::Shutdown() {
	isTerminate_ = true; //!< スレッドの終了フラグを立てる

	if (thread_.joinable()) {
		StreamLogger::Debug("Platform::InputSystem | waiting for input thread to terminate...");
		thread_.join(); //!< スレッドの終了を待つ
	}

	StreamLogger::Info("Platform::InputSystem | input system shutdown.");
}

void InputSystem::Update() {
	if (mode_ == Mode::Main) {
		UpdateInput(); //!< メインスレッドで更新する場合は直接更新
	}

	CommitInput();
}

void InputSystem::SetWindow(HWND hwnd) {
	//!< dinputのwindow設定
	keyboard_.SetWindow(hwnd);
	mouse_.SetWindow(hwnd);
}

void InputSystem::UpdateInput() {
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

void InputSystem::CommitInput() {
	//!< stackのinputをcurrentに反映させる

	//!< dinputの更新
	keyboard_.CommitStackInput();
	mouse_.CommitStackInput();

	//!< xinputの更新
	for (auto& gamepad : gamepads_) {
		gamepad.CommitStackInput();
	}
}
