#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* input
#include "InputUtil.h"

//* externals
#include <magic_enum.hpp>

//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// KeyId enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class KeyId : uint8_t {

	//* 数字キー
	KEY_0 = DIK_0,
	KEY_1 = DIK_1,
	KEY_2 = DIK_2,
	KEY_3 = DIK_3,
	KEY_4 = DIK_4,
	KEY_5 = DIK_5,
	KEY_6 = DIK_6,
	KEY_7 = DIK_7,
	KEY_8 = DIK_8,
	KEY_9 = DIK_9,

	//* アルファベットキー
	A = DIK_A,
	B = DIK_B,
	C = DIK_C,
	D = DIK_D,
	E = DIK_E,
	F = DIK_F,
	G = DIK_G,
	H = DIK_H,
	I = DIK_I,
	J = DIK_J,
	K = DIK_K,
	L = DIK_L,
	M = DIK_M,
	N = DIK_N,
	O = DIK_O,
	P = DIK_P,
	Q = DIK_Q,
	R = DIK_R,
	S = DIK_S,
	T = DIK_T,
	U = DIK_U,
	V = DIK_V,
	W = DIK_W,
	X = DIK_X,
	Y = DIK_Y,
	Z = DIK_Z,

	//* ファンクションキー
	F1  = DIK_F1,
	F2  = DIK_F2,
	F3  = DIK_F3,
	F4  = DIK_F4,
	F5  = DIK_F5,
	F6  = DIK_F6,
	F7  = DIK_F7,
	F8  = DIK_F8,
	F9  = DIK_F9,
	F10 = DIK_F10,
	F11 = DIK_F11,
	F12 = DIK_F12,

	//* 矢印キー
	Up    = DIK_UP,
	Down  = DIK_DOWN,
	Left  = DIK_LEFT,
	Right = DIK_RIGHT,

	//* 特殊キー
	Escape    = DIK_ESCAPE,
	Tab       = DIK_TAB,
	CapsLock  = DIK_CAPSLOCK,
	LShift    = DIK_LSHIFT,
	RShift    = DIK_RSHIFT,
	LControl  = DIK_LCONTROL,
	RControl  = DIK_RCONTROL,
	LAlt      = DIK_LALT,
	RAlt      = DIK_RALT,
	Space     = DIK_SPACE,
	Enter     = DIK_RETURN,
	Backspace = DIK_BACK,
	Insert    = DIK_INSERT,
	Delete    = DIK_DELETE,
	Home      = DIK_HOME,
	End       = DIK_END,
	PageUp    = DIK_PGUP,
	PageDown  = DIK_PGDN,

	//* テンキー
	Numpad0           = DIK_NUMPAD0,
	Numpad1           = DIK_NUMPAD1,
	Numpad2           = DIK_NUMPAD2,
	Numpad3           = DIK_NUMPAD3,
	Numpad4           = DIK_NUMPAD4,
	Numpad5           = DIK_NUMPAD5,
	Numpad6           = DIK_NUMPAD6,
	Numpad7           = DIK_NUMPAD7,
	Numpad8           = DIK_NUMPAD8,
	Numpad9           = DIK_NUMPAD9,
	NumpadEnter       = DIK_NUMPADENTER,
	NumpadAdd         = DIK_ADD,
	NumpadSubtract    = DIK_SUBTRACT,
	NumpadMultiply    = DIK_MULTIPLY,
	NumpadDivide      = DIK_DIVIDE,
	NumpadDecimal     = DIK_DECIMAL,

	//* その他のキー
	PrintScreen = DIK_SYSRQ,
	ScrollLock  = DIK_SCROLL,
	Pause       = DIK_PAUSE,
	Semicolon   = DIK_SEMICOLON,
	Equals      = DIK_EQUALS,
	Comma       = DIK_COMMA,
	Minus       = DIK_MINUS,
	Period      = DIK_PERIOD,
	Slash       = DIK_SLASH,
	Backtick    = DIK_GRAVE,
	LBracket    = DIK_LBRACKET,
	RBracket    = DIK_RBRACKET,
	Backslash   = DIK_BACKSLASH,
	Apostrophe  = DIK_APOSTROPHE,

	//* メディアキー (一部環境依存)
	MediaPlayPause = DIK_MEDIASELECT,
	MediaNext      = DIK_NEXTTRACK,
	MediaPrev      = DIK_PREVTRACK,
	VolumeUp       = DIK_VOLUMEUP,
	VolumeDown     = DIK_VOLUMEDOWN,
	Mute           = DIK_MUTE,
};

////////////////////////////////////////////////////////////////////////////////////////////
// MouseId enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class MouseId : uint8_t {
	Left,
	Right,
	Middle,

	Button4,
	Button5,
	Button6,
	Button7,
	Button8,
};

////////////////////////////////////////////////////////////////////////////////////////////
// GamepadButtonId enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class GamepadButtonId : uint32_t {
	//* Directional Pad (D-Pad)
	Up    = XINPUT_GAMEPAD_DPAD_UP,
	Down  = XINPUT_GAMEPAD_DPAD_DOWN,
	Left  = XINPUT_GAMEPAD_DPAD_LEFT,
	Right = XINPUT_GAMEPAD_DPAD_RIGHT,

	//* Special Buttonsk
	Back  = XINPUT_GAMEPAD_BACK,
	Start = XINPUT_GAMEPAD_START,

	//* Thumbstick Buttons
	LS = XINPUT_GAMEPAD_LEFT_THUMB,
	RS = XINPUT_GAMEPAD_RIGHT_THUMB,

	//* Shoulder Buttons
	LB = XINPUT_GAMEPAD_LEFT_SHOULDER,
	RB = XINPUT_GAMEPAD_RIGHT_SHOULDER,

	//* Face Buttons
	A = XINPUT_GAMEPAD_A,
	B = XINPUT_GAMEPAD_B,
	X = XINPUT_GAMEPAD_X,
	Y = XINPUT_GAMEPAD_Y,
};

template <> //!< magic_enumの範囲を調整
struct magic_enum::customize::enum_range<GamepadButtonId> {
	static constexpr auto min = XINPUT_GAMEPAD_DPAD_UP;
	static constexpr auto max = XINPUT_GAMEPAD_Y;
};

template<>
constexpr magic_enum::customize::customize_t magic_enum::customize::enum_name<GamepadButtonId>(GamepadButtonId value) noexcept {
	switch (value) {
		case GamepadButtonId::Up:
		case GamepadButtonId::Down:
		case GamepadButtonId::Left:
		case GamepadButtonId::Right:
		case GamepadButtonId::Back:
		case GamepadButtonId::Start:
		case GamepadButtonId::LS:
		case GamepadButtonId::RS:
		case GamepadButtonId::LB:
		case GamepadButtonId::RB:
		case GamepadButtonId::A:
		case GamepadButtonId::B:
		case GamepadButtonId::X:
		case GamepadButtonId::Y:
			return default_tag;

		default:
			return invalid_tag;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// GamepadTriggerId enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class GamepadTriggerId : uint8_t {
	Left,
	Right,
};

////////////////////////////////////////////////////////////////////////////////////////////
// GamepadStickId enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class GamepadStickId : uint8_t {
	Left,
	Right,
};

