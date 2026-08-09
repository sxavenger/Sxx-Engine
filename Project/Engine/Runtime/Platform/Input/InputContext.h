#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* input
#include "InputUtil.h"
#include "InputId.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Configuration/Configuration.h>

//* c++
#include <thread>
#include <atomic>
#include <mutex>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Platform)

////////////////////////////////////////////////////////////////////////////////////////////
// InputContext class
////////////////////////////////////////////////////////////////////////////////////////////
class InputContext final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mode enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Mode : bool {
		Sync, //!< メインスレッドで更新
		Async //!< 非同期スレッドで更新
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Settings structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Settings {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Parse(const toml::table& config);

		static Settings ParseFromConfig(const Configuration& config);

		static void Log(const Settings& settings);

		//=========================================================================================
		// public variables
		//=========================================================================================

		static inline constexpr Configuration::Path kConfigPath{ "Platform.InputContext" }; //!< 設定のパス.

		Mode mode = Mode::Sync; //!< 入力の更新モード.

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(const Configuration& config);

	void Shutdown();

	void Update();

	void SetWindow(HWND hwnd);

	//* key input option *//

	const Keyboard& GetKeyboard() const { return keyboard_; }

	//* mouse input option *//

	const Mouse& GetMouse() const { return mouse_; }

	//* gamepad input option *//

	const Gamepad& GetGamepad(uint8_t number) const { return gamepads_[number]; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* dinput *//

	ComPtr<IDirectInput8> directInput_;

	Keyboard keyboard_;
	Mouse mouse_;

	//* xinput *//

	std::array<Gamepad, XUSER_MAX_COUNT> gamepads_;

	//* input thread *//

	std::thread thread_;
	std::mutex mutex_;

	std::atomic<bool> running_ = false;

	//* settings *//

	Settings settings_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* input thread method *//

	void UpdateInput();

	void CommitInput();

};

SXAVENGER_ENGINE_NAMESPACE_END
