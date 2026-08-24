#include "Gamepad.h"
SXAVENGER_ENGINE_USING_(Platform)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [Gamepad] InputData structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Gamepad::InputData::Clear() {
	*this = InputData();
}

void Gamepad::InputData::StackState(const XINPUT_STATE& current) {
	if (xinput.dwPacketNumber == current.dwPacketNumber) {
		return; //!< 前回と同じ状態
	}

	xinput.Gamepad.wButtons      |= current.Gamepad.wButtons;
	xinput.Gamepad.bLeftTrigger  = std::max(xinput.Gamepad.bLeftTrigger,  current.Gamepad.bLeftTrigger);
	xinput.Gamepad.bRightTrigger = std::max(xinput.Gamepad.bRightTrigger, current.Gamepad.bRightTrigger);

	xinput.Gamepad.sThumbLX = current.Gamepad.sThumbLX;
	xinput.Gamepad.sThumbLY = current.Gamepad.sThumbLY;
	xinput.Gamepad.sThumbRX = current.Gamepad.sThumbRX;
	xinput.Gamepad.sThumbRY = current.Gamepad.sThumbRY;
}

bool Gamepad::InputData::IsConnect() const {
	return isConnect;
}

bool Gamepad::InputData::GetButton(GamepadButtonId id) const {
	return (xinput.Gamepad.wButtons & static_cast<uint16_t>(id)) != 0;
}

bool Gamepad::InputData::GetButton(GamepadTriggerId id) const {
	switch (id) {
		case GamepadTriggerId::Left:
			return xinput.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

		case GamepadTriggerId::Right:
			return xinput.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

		default:
			STREAM_LOG_WARNING(
				"Platform::Gamepad | GamepadTriggerId invalid. id: {}", static_cast<uint8_t>(id)
			);
			return false; //!< 無効なidの場合はfalseを返す
	}
}

Vector2i Gamepad::InputData::GetStick(GamepadStickId id) const {
	switch (id) {
		case GamepadStickId::Left:
			return { xinput.Gamepad.sThumbLX, xinput.Gamepad.sThumbLY };

		case GamepadStickId::Right:
			return { xinput.Gamepad.sThumbRX, xinput.Gamepad.sThumbRY };

		default:
			STREAM_LOG_WARNING(
				"Platform::Gamepad | GamepadStickId invalid. id: {}", static_cast<uint8_t>(id)
			);
			return Vector2i::Origin(); //!< 無効なidの場合は原点を返す
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Gamepad class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Gamepad::Init(uint8_t number) {
	STREAM_ASSERT(number < XUSER_MAX_COUNT, "gamepad number is out of range.");

	//!< numberの設定
	number_ = number;

	STREAM_LOG_INFO("Platform::Gamepad | initialized. gamepad number: {}", number_);
}

void Gamepad::Update() {

	InputData& input = inputs_[static_cast<uint8_t>(InputUtil::Buffer::Stack)];

	//!< コントローラーの入力状態を取得
	XINPUT_STATE current = {};

	auto dr = XInputGetState(number_, &current);
	input.isConnect = (dr == ERROR_SUCCESS); //!< 接続されているかどうかの設定

	if (!input.isConnect) {
		//!< 接続されてない場合, 状態をクリア
		input.Clear();
		return;
	}

	//!< 現在の状態にスタックさせる
	input.StackState(current);
}

void Gamepad::CommitStackInput() {
	//!< Previousの状態を更新
	inputs_[static_cast<uint8_t>(InputUtil::Buffer::Previous)]
		= inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)];

	//!< Currentの状態を更新
	inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)]
		= inputs_[static_cast<uint8_t>(InputUtil::Buffer::Stack)];

	//!< Stackの状態をクリア
	inputs_[static_cast<uint8_t>(InputUtil::Buffer::Stack)].Clear();
}

bool Gamepad::IsConnect(InputUtil::Buffer buffer) const {
	return inputs_[static_cast<uint8_t>(buffer)].IsConnect();
}

InputUtil::StateView Gamepad::GetButton(GamepadButtonId id) const {
	if (!IsConnect(InputUtil::Buffer::Current)) {
		return InputUtil::State::None; //!< 接続されてない場合はNoneを返す
	}

	return InputUtil::GetInputState(
		inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)].GetButton(id),
		inputs_[static_cast<uint8_t>(InputUtil::Buffer::Previous)].GetButton(id)
	);
}

InputUtil::StateView Gamepad::GetButton(GamepadTriggerId id) const {
	if (!IsConnect(InputUtil::Buffer::Current)) {
		return InputUtil::State::None; //!< 接続されてない場合はNoneを返す
	}

	return InputUtil::GetInputState(
		inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)].GetButton(id),
		inputs_[static_cast<uint8_t>(InputUtil::Buffer::Previous)].GetButton(id)
	);
}

Vector2i Gamepad::GetStick(GamepadStickId id) const {
	if (!IsConnect(InputUtil::Buffer::Current)) {
		return Vector2i::Origin(); //!< 接続されてない場合は原点を返す
	}

	return inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)].GetStick(id);
}

Vector2f Gamepad::GetStickNormalized(GamepadStickId id) const {
	if (!IsConnect(InputUtil::Buffer::Current)) {
		return Vector2f::Origin(); //!< 接続されてない場合は原点を返す
	}

	return static_cast<Vector2f>(GetStick(id)) / static_cast<float>(SHRT_MAX);
}
