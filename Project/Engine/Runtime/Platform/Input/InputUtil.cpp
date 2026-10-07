#include "InputUtil.h"
SXAVENGER_ENGINE_USING_(Platform)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <bitset>

////////////////////////////////////////////////////////////////////////////////////////////
// InputUtil namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

InputUtil::State InputUtil::State::Determine(bool current, bool previous) {
	std::bitset<2> state = {};
	state[0] = current;  //!< 現在の状態
	state[1] = previous; //!< 1フレーム前の状態

	switch (state.to_ulong()) {
		case 0b01: //!< currentのみ押されている場合.
			return Condition::Trigger;

		case 0b11: //!< currentもpreviousも押されている場合.
			return Condition::Hold;

		case 0b10: //!< previousのみ押されている場合.
			return Condition::Release;

		default: //!< currentもpreviousも押されていない場合.
			return Condition::None;
	}
}
