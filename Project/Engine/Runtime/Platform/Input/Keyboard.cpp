#include "Keyboard.h"
SXAVENGER_ENGINE_USING_(Platform)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <bitset>

////////////////////////////////////////////////////////////////////////////////////////////
// [Keyboard] InputData structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Keyboard::InputData::Clear() {
	*this = InputData();
}

void Keyboard::InputData::StackState(const KeyInput& current) {
	for (size_t i = 0; i < kKeyCount; ++i) {
		keys[i] |= current[i];
	}
}

bool Keyboard::InputData::IsEnableAcquire() const {
	return isEnableAcquire;
}

bool Keyboard::InputData::GetKey(KeyId id) const {
	return keys[static_cast<uint8_t>(id)] != 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Keyboard::Init(IDirectInput8* dinput) {
	//!< Deviceの生成
	auto hr = dinput->CreateDevice(
		GUID_SysKeyboard, &device_, NULL
	);
	ComPtrUtil::Assert(hr, L"failed to create keyboard device.");

	// 入力データ形式のセット
	hr = device_->SetDataFormat(
		&c_dfDIKeyboard // 標準形式
	);
	ComPtrUtil::Assert(hr, L"keyboard set data format failed.");

	STREAM_LOG_INFO("Platform::Keyboard | keyboard device created.");
}

void Keyboard::Update() {
	
	InputData& input = inputs_[static_cast<uint8_t>(InputUtil::Buffer::Stack)];

	input.isEnableAcquire = SetCooperativeLevel(hwnd_);

	if (!input.isEnableAcquire) {
		//!< windowが非アクティブで入力が取得できない場合は状態をクリアして終了
		input.Clear();
		return;
	}

	//!< キーボード情報の取得開始
	device_->Acquire();

	//!< キーボードの入力状態を取得
	KeyInput current = {};
	device_->GetDeviceState(sizeof(current), current.data());

	//!< 現在の状態をスタックさせる
	input.StackState(current);

}

void Keyboard::CommitStackInput() {
	//!< Previousの状態を更新
	inputs_[static_cast<uint8_t>(InputUtil::Buffer::Previous)]
		= inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)];

	//!< Currentの状態を更新
	inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)]
		= inputs_[static_cast<uint8_t>(InputUtil::Buffer::Stack)];

	//!< Stackの状態をクリア
	inputs_[static_cast<uint8_t>(InputUtil::Buffer::Stack)].Clear();
}

bool Keyboard::IsEnableAcquire(InputUtil::Buffer buffer) const {
	return inputs_[static_cast<uint8_t>(buffer)].IsEnableAcquire();
}

InputUtil::StateView Keyboard::GetKey(KeyId id) const {
	if (!IsEnableAcquire(InputUtil::Buffer::Current)) {
		//!< windowが非アクティブで入力が取得できない場合は入力なし
		return InputUtil::State::None;
	}

	return InputUtil::GetInputState(
		inputs_[static_cast<uint8_t>(InputUtil::Buffer::Current)].GetKey(static_cast<KeyId>(id)),
		inputs_[static_cast<uint8_t>(InputUtil::Buffer::Previous)].GetKey(static_cast<KeyId>(id))
	);
}

bool Keyboard::SetCooperativeLevel(HWND hwnd) {
	if (hwnd == nullptr) {
		return false;
	}

	if (hwnd != current_) {
		//* 現在のhwndと違う場合, 再設定
		// 排他制御レベルのセット
		auto hr = device_->SetCooperativeLevel(
			hwnd,
			static_cast<DWORD>(flags_)
		);

		if (FAILED(hr)) {
			STREAM_LOG_ERROR(
				L"Platform::InputMouse | failed to set cooperative level. hwnd: {:p} _com_error: {}",
				static_cast<const void*>(hwnd), ComPtrUtil::GetComErrorMessage(hr)
			);
			return false; //!< cooperative levelの設定に失敗した場合は、入力が取得できないためfalseを返す
		}

		current_ = hwnd; //!< hwndの更新
		STREAM_LOG_INFO(
			"Platform::Keyboard | keyboard cooperative level set. hwnd: {:p}",
			static_cast<const void*>(hwnd)
		);
	}

	return true;
}
