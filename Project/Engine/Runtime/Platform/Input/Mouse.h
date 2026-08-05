#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* input
#include "InputUtil.h"
#include "InputId.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Utility/ComPtr.h>

//* lib
#include <Lib/Flag/Flag.h>
#include <Lib/Math/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Platform)

////////////////////////////////////////////////////////////////////////////////////////////
// Mouse class
////////////////////////////////////////////////////////////////////////////////////////////
class Mouse final {
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

		void StackState(const DIMOUSESTATE2& current);

		bool IsEnableAcquire() const;

		Vector2i GetDeltaMove() const;

		bool IsWheel() const;

		int32_t GetDeltaWheel() const;

		bool GetButton(MouseId id) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		bool isEnableAcquire = false; //!< inputが取得可能かどうか
		DIMOUSESTATE2 mouse  = {};

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief Mouse Device の初期化
	void Init(IDirectInput8* dinput);

	//! @brief inputの更新
	void Update();

	//! @brief StackしたinputをCurrentに反映させる
	void CommitStackInput();

	//! @brief CooperativeLevelの設定に使用するwindowを設定する
	void SetWindow(HWND hwnd) { hwnd_ = hwnd; }

	//* mouse input option *//

	//! @brief mouseが有効状態か確認する
	bool IsEnableAcquire(InputUtil::Buffer buffer = InputUtil::Buffer::Current) const;

	//! @brief mouseの位置を取得する(全画面基準)
	Vector2i GetPosition() const;

	//! @brief mouseの位置を取得する(設定されているwindow基準)
	Vector2i GetScreenPosition() const;

	//! @brief mouseの移動量を取得する
	Vector2i GetDeltaMove() const;

	//! @brief mouseの位置を設定する(全画面基準)
	void SetPosition(const Vector2i& position) const;

	//! @brief mouseの位置を設定する(設定されているwindow基準)
	void SetScreenPosition(const Vector2i& position) const;

	//! @brief mouseの入力状態を取得する
	InputUtil::StateView GetButton(MouseId id) const;

	//! @brief ホイールが回転しているか
	bool IsWheel() const;

	//! @brief ホイールが回転しているかを取得する (回転方向が上)
	//! @return ホイールの回転量を[-WHEEL_DELTA, WHEEL_DELTA]の範囲で返す。回転していない場合は0を返す。
	int32_t GetDeltaWheel() const;

	//! @brief ホイールが回転しているか正規化された値で取得する
	//! @return ホイールの回転量を[-1.0f, 1.0f]の範囲で返す。回転していない場合は0.0fを返す。
	float GetDeltaWheelNormalized() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input device *//

	ComPtr<IDirectInputDevice8> device_;

	FlagEnum<InputUtil::CooperativeFlag> flags_ = InputUtil::CooperativeFlag::Default;
	HWND hwnd_ = nullptr; //!< 現在のhwnd, CooperativeLevelの設定に使用

	//* runtime state *//

	HWND current_ = nullptr; //!< CooperativeLevelに設定されているwindow.

	//* input state *//

	std::array<InputData, InputUtil::kBufferCount> inputs_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	bool SetCooperativeLevel(HWND hwnd);

};

SXAVENGER_ENGINE_NAMESPACE_END
