#pragma once

//-----------------------------------------------------------------------------------------
// include
//----------------------------------------------------------------------------------------
//* assets
#include "AssimpOption.h"
#include "AssimpNodeAnimation.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Time/TimePoint.h>

//* assimp
#include <assimp/anim.h>

//* c++
#include <string_view>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpAnimation class
////////////////////////////////////////////////////////////////////////////////////////////
class AssimpAnimation final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	AssimpAnimation() = delete;
	AssimpAnimation(const aiAnimation* animation) noexcept : animation_(animation) {}

	//* animation option *//

	std::string_view GetName() const;

	//! @brief アニメーションの全体時間を取得する
	TimePointd<TimeUnit::Second> GetDuration() const;

	double GetTicksPerSecond() const;

	uint32_t GetChannelCount() const;

	AssimpNodeAnimation GetChannel(uint32_t index) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	RefPtr<const aiAnimation> animation_ = nullptr;

};

SXAVENGER_ENGINE_NAMESPACE_END
