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
	// State structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct State final {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Condition enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Condition : uint8_t {
			None,    //!< 入力なし
			Trigger, //!< トリガー (押された瞬間)
			Hold,    //!< ホールド (押されている状態)
			Release  //!< リリース (離された瞬間)
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		constexpr State() noexcept = default;

		//* condition option *//

		constexpr bool IsPress() const { return condition == Condition::Trigger || condition == Condition::Hold; }

		constexpr bool IsTrigger() const { return condition == Condition::Trigger; }

		constexpr bool IsHold() const { return condition == Condition::Hold; }

		constexpr bool IsRelease() const { return condition == Condition::Release; }

		//* operator [assignment] <Condition> *//

		constexpr State(Condition rhs) noexcept : condition(rhs) {}
		constexpr State& operator=(Condition rhs) noexcept { condition = rhs; return *this; }


		//* operator [comparison] <State> *//

		constexpr bool operator==(const State& rhs) const noexcept { return condition == rhs.condition; }
		constexpr bool operator!=(const State& rhs) const noexcept { return condition != rhs.condition; }

		//* operator [comparison] <Condition> *//

		constexpr bool operator==(const Condition& rhs) const noexcept { return condition == rhs; }
		constexpr bool operator!=(const Condition& rhs) const noexcept { return condition != rhs; }

		//* operator [cast] <Condition> *//

		constexpr operator Condition() const noexcept { return condition; }

		//* static methods *//

		static State Determine(bool current, bool previous);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Condition condition = Condition::None; //!< 入力状態

	};
	
}

SXAVENGER_ENGINE_NAMESPACE_END
