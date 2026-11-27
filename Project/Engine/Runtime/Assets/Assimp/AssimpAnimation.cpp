#include "AssimpAnimation.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpAnimation class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string_view AssimpAnimation::GetName() const {
	const aiString& name = animation_->mName;
	return std::string_view(name.C_Str(), name.length);
}

TimePointd<TimeUnit::Second> AssimpAnimation::GetDuration() const {
	return AssimpNodeAnimation::ConvertTimePoint(animation_->mDuration, animation_->mTicksPerSecond);
}

double AssimpAnimation::GetTicksPerSecond() const {
	return animation_->mTicksPerSecond;
}

uint32_t AssimpAnimation::GetChannelCount() const {
	return animation_->mNumChannels;
}

AssimpNodeAnimation AssimpAnimation::GetChannel(uint32_t index) const {
	return animation_->mChannels[index];
}
