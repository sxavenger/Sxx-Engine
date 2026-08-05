#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* input
#define DIRECTINPUT_VERSION 0x0800 //!< DirectInputのversion指定
#include <dinput.h>
#include <Xinput.h>

//* c++
#include <cstdint>

//-----------------------------------------------------------------------------------------
// pragma comment
//-----------------------------------------------------------------------------------------
//!< dinput
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//!< xinput
#pragma comment(lib, "Xinput.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Platform)

////////////////////////////////////////////////////////////////////////////////////////////
// InputUtil namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace InputUtil {

	////////////////////////////////////////////////////////////////////////////////////////////
	// State enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class State : uint8_t {
		None,    //!< 入力なし
		Trigger, //!< トリガー (押された瞬間)
		Hold,    //!< ホールド (押されている状態)
		Release  //!< リリース (離された瞬間)
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Buffer enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Buffer : uint8_t {
		Current,  //!< 現在の状態
		Previous, //!< 1フレーム前の状態
		Stack,    //!< 現在の状態のスタック用
	};
	static const size_t kBufferCount = static_cast<size_t>(Buffer::Stack) + 1; //!< bufferの数

	////////////////////////////////////////////////////////////////////////////////////////////
	// CooperativeFlag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//!< DirectInputのCooperativeLevelのフラグ
	enum class CooperativeFlag : DWORD {
		Foreground   = DISCL_FOREGROUND,   //!< 画面が手前にある場合のみ入力を受け付け
		Background   = DISCL_BACKGROUND,   //!< 画面が手前になくても入力を受け付け
		Exclusive    = DISCL_EXCLUSIVE,    //!< デバイスをこのアプリで占有する
		NonExclusive = DISCL_NONEXCLUSIVE, //!< デバイスをこのアプリで占有しない

		Default = NonExclusive | Foreground,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	State GetInputState(bool current, bool previous);

	////////////////////////////////////////////////////////////////////////////////////////////
	// StateView class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 入力状態を簡単に確認するためのクラス
	struct StateView {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		constexpr StateView() noexcept = default;

		//* state option *//

		constexpr bool IsPress() const { return state == State::Trigger || state == State::Hold; }

		constexpr bool IsTrigger() const { return state == State::Trigger; }

		constexpr bool IsHold() const { return state == State::Hold; }

		constexpr bool IsRelease() const { return state == State::Release; }

		//* operatpr [copy] <StateView> *//

		constexpr StateView(const StateView&) noexcept            = default;
		constexpr StateView& operator=(const StateView&) noexcept = default;

		//* operator [assignment] <State> *//

		constexpr StateView(State _state) noexcept : state(_state) {}
		constexpr StateView& operator=(State _state) noexcept { state = _state; return *this; }

		//* operator [comparison] <StateView> *//

		constexpr bool operator==(const StateView& rhs) const noexcept { return state == rhs.state; }
		constexpr bool operator!=(const StateView& rhs) const noexcept { return state != rhs.state; }

		//* operator [comparison] <State> *//

		constexpr bool operator==(State rhs) const noexcept { return state == rhs; }
		constexpr bool operator!=(State rhs) const noexcept { return state != rhs; }

		//* operator [cast] <State> *//

		constexpr operator State() const noexcept { return state; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		State state = State::None; //!< 入力状態
		
	};
}

SXAVENGER_ENGINE_NAMESPACE_END
