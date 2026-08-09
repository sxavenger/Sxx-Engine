#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* input
#include "InputId.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"
#include "InputContext.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Configuration/Configuration.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Platform)

////////////////////////////////////////////////////////////////////////////////////////////
// Input class
////////////////////////////////////////////////////////////////////////////////////////////
class Input {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init(const Configuration& config);

	static void Shutdown();

	//* keyboard input option *//

	static const Keyboard& GetKeyboard() { return context_.GetKeyboard(); }

	//* mouse input option *//

	static const Mouse& GetMouse() { return context_.GetMouse(); }

	//* gamepad input option *//

	static const Gamepad& GetGamepad(uint8_t number) { return context_.GetGamepad(number); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline InputContext context_; //!< 入力コンテキスト

};

SXAVENGER_ENGINE_NAMESPACE_END
