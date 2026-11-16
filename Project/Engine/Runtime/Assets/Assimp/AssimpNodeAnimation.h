#pragma once

//-----------------------------------------------------------------------------------------
// include
//----------------------------------------------------------------------------------------
//* assets
#include "AssimpOption.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Time/TimePoint.h>
#include <Lib/Math/Vector3.h>
#include <Lib/Math/Quaternion.h>

//* assimp
#include <assimp/anim.h>

//* c++
#include <string_view>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpNodeAnimation class
////////////////////////////////////////////////////////////////////////////////////////////
class AssimpNodeAnimation final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Key structure
	////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	struct Key final {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		TimePointd<TimeUnit::Second> time;  //!< キーフレーム時刻
		T                            value; //!< 値
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	AssimpNodeAnimation() = delete;
	AssimpNodeAnimation(const aiNodeAnim* nodeAnimation) noexcept : nodeAnimation_(nodeAnimation) {}

	//* animation option *//

	std::string_view GetNodeName() const;

	uint32_t GetPositionKeyCount() const;

	Key<Vector3f> GetPositionKey(uint32_t index, double ticksPerSecond) const;

	uint32_t GetRotationKeyCount() const;

	Key<Quaternionf> GetRotationKey(uint32_t index, double ticksPerSecond) const;

	uint32_t GetScaleKeyCount() const;

	Key<Vector3f> GetScaleKey(uint32_t index, double ticksPerSecond) const;

	static TimePointd<TimeUnit::Second> ConvertTimePoint(double duration, double ticksPerSecond);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	RefPtr<const aiNodeAnim> nodeAnimation_ = nullptr;

};

SXAVENGER_ENGINE_NAMESPACE_END
