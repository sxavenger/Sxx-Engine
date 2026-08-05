#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* input
#include "InputUtil.h"
#include "InputId.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Vector2.h>

//* xinput
#include <Xinput.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Platform)

////////////////////////////////////////////////////////////////////////////////////////////
// Gamepad class
////////////////////////////////////////////////////////////////////////////////////////////
class Gamepad final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// InputData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct InputData {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Clear();

		void StackState(const XINPUT_STATE& current);

		bool IsConnect() const;

		bool GetButton(GamepadButtonId id) const;

		bool GetButton(GamepadTriggerId id) const;

		Vector2i GetStick(GamepadStickId id) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		bool isConnect      = false; //!< gamepadが接続されているかどうか
		XINPUT_STATE xinput = {};    //!< xinput state

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(uint8_t number);

	//! @brief inputの更新
	void Update();

	//! @brief StackしたinputをCurrentに反映させる
	void CommitStackInput();

	//* gamepad input option *//

	//! @brief コントローラーが接続されているか確認する
	bool IsConnect(InputUtil::Buffer buffer = InputUtil::Buffer::Current) const;

	InputUtil::StateView GetButton(GamepadButtonId id) const;
	InputUtil::StateView GetButton(GamepadTriggerId id) const;

	//! @brief スティックの値を取得する
	//! @return スティックの値[-SHRT_MAX, SHRT_MAX]
	Vector2i GetStick(GamepadStickId id) const;

	//! @brief スティックの値を正規化して取得する
	//! @return 正規化されたスティックの値[-1.0f, 1.0f]
	Vector2f GetStickNormalized(GamepadStickId id) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* xinput *//

	uint8_t number_; //!< コントローラー番号[0 ~ XUSER_MAX_COUNT - 1]

	//* input state *//

	std::array<InputData, InputUtil::kBufferCount> inputs_;

};

SXAVENGER_ENGINE_NAMESPACE_END
