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

//* dinput
#include <dinput.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Platform)

////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard class
////////////////////////////////////////////////////////////////////////////////////////////
class Keyboard final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// constants
	////////////////////////////////////////////////////////////////////////////////////////////

	const static size_t kKeyCount = 256; //!< keyの数
	using KeyInput = std::array<BYTE, kKeyCount>; //!< key input data type

	////////////////////////////////////////////////////////////////////////////////////////////
	// InputData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct InputData {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Clear();

		void StackState(const KeyInput& current);
		
		bool IsEnableAcquire() const;

		bool GetKey(KeyId id) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		bool isEnableAcquire = false; //!< inputが取得可能かどうか
		KeyInput keys        = {};    //!< keyの状態

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief Keyboard Device の初期化
	void Init(IDirectInput8* dinput);

	//! @brief inputの更新
	void Update();

	//! @brief StackしたinputをCurrentに反映させる
	void CommitStackInput();

	//! @brief CooperativeLevelの設定に使用するwindowを設定する
	void SetWindow(HWND hwnd) { hwnd_ = hwnd; }

	//* key input option *//

	//! @brief keyが有効状態か確認する
	bool IsEnableAcquire(InputUtil::Buffer buffer = InputUtil::Buffer::Current) const;
	
	//! @brief keyの入力状態を取得する
	InputUtil::StateView GetKey(KeyId id) const;

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
