#include "Mouse.h"
SXAVENGER_ENGINE_USING_(Platform)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [Mouse] InputData structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Mouse::InputData::Clear() {
	*this = InputData();
}

void Mouse::InputData::StackState(const DIMOUSESTATE2& current) {
	
	for (size_t i = 0; i < 8; ++i) {
		mouse.rgbButtons[i] |= current.rgbButtons[i];
	}

	mouse.lX += current.lX;
	mouse.lY += current.lY;
	mouse.lZ = std::clamp<int32_t>(mouse.lZ + current.lZ, -WHEEL_DELTA, WHEEL_DELTA);
	// FIXME: ホイールの加算値が期待している数値にならない.
}

bool Mouse::InputData::IsEnableAcquire() const {
	return isEnableAcquire;
}

Vector2i Mouse::InputData::GetDeltaMove() const {
	return { mouse.lX, mouse.lY };
}

bool Mouse::InputData::IsWheel() const {
	return mouse.lZ != 0;
}

int32_t Mouse::InputData::GetDeltaWheel() const {
	return mouse.lZ;
}

bool Mouse::InputData::GetButton(MouseId id) const {
	return mouse.rgbButtons[static_cast<uint8_t>(id)] != 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Mouse class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Mouse::Init(IDirectInput8* dinput) {

	//!< Device の生成
	auto hr = dinput->CreateDevice(
		GUID_SysMouse, &device_, NULL
	);
	ComPtrUtil::Assert(hr, L"failed to create mouse device.");

	//!< 入力データ形式のセット
	hr = device_->SetDataFormat(
		&c_dfDIMouse2 // 標準形式
	);
	ComPtrUtil::Assert(hr, L"mouse set data format failed.");

	StreamLogger::Info("Platform::Mouse | mouse device created.");
}

void Mouse::Update() {

	InputData& input = inputs_[static_cast<uint8_t>(InputUtil::Buffer::Stack)];

	input.isEnableAcquire = SetCooperativeLevel(hwnd_);

	if (!input.isEnableAcquire) {
		input.Clear();
		return;
	}

	//!< Mouse情報の取得開始
	device_->Acquire();

	//!< Mouseの入力状態を取得
	DIMOUSESTATE2 current = {};
	device_->GetDeviceState(sizeof(current), &current);

	//!< 現在の状態にスタックさせる
	input.StackState(current);
}

void Mouse::CommitStackInput() {
	//!< Previousの状態を更新
	inputs_[static_cast<uint8_t>(InputUtil::Buffer::Previous)]
		= inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)];

	//!< Currentの状態を更新
	inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)]
		= inputs_[static_cast<uint8_t>(InputUtil::Buffer::Stack)];

	//!< Stackの状態をクリア
	inputs_[static_cast<uint8_t>(InputUtil::Buffer::Stack)].Clear();
}

bool Mouse::IsEnableAcquire(InputUtil::Buffer buffer) const {
	return inputs_[static_cast<uint8_t>(buffer)].IsEnableAcquire();
}

Vector2i Mouse::GetPosition() const {
	if (!IsEnableAcquire(InputUtil::Buffer::Current)) {
		return Vector2i::Origin(); //!< 入力が取得できない場合は原点を返す
	}

	POINT point = {};
	GetCursorPos(&point);

	return { point.x, point.y };
}

Vector2i Mouse::GetScreenPosition() const {
	if (!IsEnableAcquire(InputUtil::Buffer::Current)) {
		return Vector2i::Origin(); //!< 入力が取得できない場合は原点を返す
	}

	POINT point = {};
	GetCursorPos(&point);

	if (hwnd_ != nullptr) {
		ScreenToClient(hwnd_, &point);
	}

	return { point.x, point.y };
}

Vector2i Mouse::GetDeltaMove() const {
	if (!IsEnableAcquire(InputUtil::Buffer::Current)) {
		return Vector2i::Origin(); //!< 入力が取得できない場合は原点を返す
	}

	return inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)].GetDeltaMove();
}

void Mouse::SetPosition(const Vector2i& position) const {
	if (!IsEnableAcquire(InputUtil::Buffer::Current)) {
		return; //!< 入力が取得できない場合は位置を設定しない
	}

	POINT point = { position.x, position.y };
	SetCursorPos(point.x, point.y);
}

void Mouse::SetScreenPosition(const Vector2i& position) const {
	if (!IsEnableAcquire(InputUtil::Buffer::Current)) {
		return; //!< 入力が取得できない場合は位置を設定しない
	}

	if (hwnd_ == nullptr) {
		return; //!< hwndが設定されていない場合は位置を設定しない
	}

	POINT point = { position.x, position.y };
	ClientToScreen(hwnd_, &point);
	SetCursorPos(point.x, point.y);
}

InputUtil::StateView Mouse::GetButton(MouseId id) const {
	if (!IsEnableAcquire(InputUtil::Buffer::Current)) {
		return InputUtil::State::None; //!< 入力が取得できない場合は入力なし
	}

	return InputUtil::GetInputState(
		inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)].GetButton(static_cast<MouseId>(id)),
		inputs_[static_cast<uint8_t>(InputUtil::Buffer::Previous)].GetButton(static_cast<MouseId>(id))
	);
}

bool Mouse::IsWheel() const {
	return inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)].IsWheel();
}

int32_t Mouse::GetDeltaWheel() const {
	return inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)].GetDeltaWheel();
}

float Mouse::GetDeltaWheelNormalized() const {
	return static_cast<float>(GetDeltaWheel()) / static_cast<float>(WHEEL_DELTA);
}

bool Mouse::SetCooperativeLevel(HWND hwnd) {
	if (hwnd == nullptr) {
		return false;
	}

	if (current_ != hwnd) {
		//* 現在のhwndと違う場合, 再設定
		// 排他制御レベルのセット
		auto hr = device_->SetCooperativeLevel(
			hwnd,
			static_cast<DWORD>(flags_)
		);
		
		if (FAILED(hr)) {
			StreamLogger::Error(
				L"Platform::Mouse | failed to set cooperative level. hwnd: {:p} _com_error: {}",
				static_cast<const void*>(hwnd), ComPtrUtil::GetComErrorMessage(hr)
			);

			return false; //!< cooperative levelの設定に失敗した場合は、入力が取得できないためfalseを返す
		}

		current_ = hwnd; //!< hwndの更新
		StreamLogger::Info("Platform::Mouse | mouse cooperative level set. hwnd: {:p}", static_cast<const void*>(hwnd));
	}

	return true;
}
