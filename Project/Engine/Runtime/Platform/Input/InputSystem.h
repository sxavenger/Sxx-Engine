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

//* c++
#include <thread>
#include <mutex>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Platform)

////////////////////////////////////////////////////////////////////////////////////////////
// InputSystem class
////////////////////////////////////////////////////////////////////////////////////////////
class InputSystem final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mode enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Mode : bool {
		Main, //!< メインスレッドで更新
		Async //!< 非同期スレッドで更新
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(Mode mode);

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

	bool isTerminate_ = false;

	//* paraemter *//

	Mode mode_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* input thread method *//

	void UpdateInput();

	void CommitInput();

};

SXAVENGER_ENGINE_NAMESPACE_END
