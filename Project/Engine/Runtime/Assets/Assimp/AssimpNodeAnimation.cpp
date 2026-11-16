#include "AssimpNodeAnimation.h"
SXAVENGER_ENGINE_USING_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpNodeAnimation class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string_view AssimpNodeAnimation::GetNodeName() const {
	const aiString& name = nodeAnimation_->mNodeName;
	return std::string_view(name.C_Str(), name.length);
}

uint32_t AssimpNodeAnimation::GetPositionKeyCount() const {
	return nodeAnimation_->mNumPositionKeys;
}

AssimpNodeAnimation::Key<Vector3f> AssimpNodeAnimation::GetPositionKey(uint32_t index, double ticksPerSecond) const {
	STREAM_ASSERT(index < nodeAnimation_->mNumPositionKeys, "index is out of bounds.");

	const aiVectorKey& key = nodeAnimation_->mPositionKeys[index];

	Key<Vector3f> output;
	output.time  = AssimpNodeAnimation::ConvertTimePoint(key.mTime, ticksPerSecond);
	output.value = { key.mValue.x, key.mValue.y, key.mValue.z };

	return output;
}

uint32_t AssimpNodeAnimation::GetRotationKeyCount() const {
	return nodeAnimation_->mNumRotationKeys;
}

AssimpNodeAnimation::Key<Quaternionf> AssimpNodeAnimation::GetRotationKey(uint32_t index, double ticksPerSecond) const {
	STREAM_ASSERT(index < nodeAnimation_->mNumRotationKeys, "index is out of bounds.");

	const aiQuatKey& key = nodeAnimation_->mRotationKeys[index];

	Key<Quaternionf> output;
	output.time  = AssimpNodeAnimation::ConvertTimePoint(key.mTime, ticksPerSecond);
	output.value = { key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w };

	return output;
}

uint32_t AssimpNodeAnimation::GetScaleKeyCount() const {
	return nodeAnimation_->mNumScalingKeys;
}

AssimpNodeAnimation::Key<Vector3f> AssimpNodeAnimation::GetScaleKey(uint32_t index, double ticksPerSecond) const {
	STREAM_ASSERT(index < nodeAnimation_->mNumScalingKeys, "index is out of bounds.");

	const aiVectorKey& key = nodeAnimation_->mScalingKeys[index];

	Key<Vector3f> output;
	output.time  = AssimpNodeAnimation::ConvertTimePoint(key.mTime, ticksPerSecond);
	output.value = { key.mValue.x, key.mValue.y, key.mValue.z };

	return output;
}

TimePointd<TimeUnit::Second> AssimpNodeAnimation::ConvertTimePoint(double duration, double ticksPerSecond) {
	return TimePointd<TimeUnit::Second>{ duration / ticksPerSecond };
	//!< assimpのキー時刻は「ティック」単位のため, ticksPerSecondで割って秒単位へ正規化する.
}
