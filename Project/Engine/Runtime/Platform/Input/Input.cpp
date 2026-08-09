#include "Input.h"
SXAVENGER_ENGINE_USING_(Platform)

////////////////////////////////////////////////////////////////////////////////////////////
// Input class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Input::Init(const Configuration& config) {
	context_.Init(config); //!< 入力コンテキストの初期化
}

void Input::Shutdown() {
	context_.Shutdown(); //!< 入力コンテキストの終了
}

void Input::Update() {
	context_.Update(); //!< 入力の更新. (Sync modeならpolling, その後currentへcommit)
}

void Input::SetWindow(HWND hwnd) {
	context_.SetWindow(hwnd); //!< dinputのCooperativeLevelに使うwindowの設定
}

const Keyboard& Input::GetKeyboard() {
	return context_.GetKeyboard(); //!< Keyboardの取得
}

const Mouse& Input::GetMouse() {
	return context_.GetMouse(); //!< Mouseの取得
}

const Gamepad& Input::GetGamepad(uint8_t number) {
	return context_.GetGamepad(number); //!< Gamepadの取得
}
