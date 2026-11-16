#include "AnimationSequence.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [AnimationSequence] Description structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Transform3d AnimationSequence::Description::SampleBoneTransform(const std::string_view& name, TimePointd<TimeUnit::Second> time) const {
	STREAM_ASSERT(boneAnimations.Contains(name), "Assets::AnimationSequence | bone animation not found.");

	const Animation& animation = boneAnimations.At(name);

	Transform3d transform = {};

	{ //!< Translationの取得
		const Animation::Curve<Vector3f>& curve      = animation.GetCurve<Vector3f>(kTranslationCurveName);
		Animation::Curve<Vector3f>::Segument segment = curve.GetSegument(time); //!< 指定した時間における補間値を取得する

		//!< 補間値を計算する
		transform.translation = Math::Lerp(segment.start, segment.end, static_cast<float>(segment.t)); 
	}

	{ //!< Rotationの取得
		const Animation::Curve<Quaternionf>& curve = animation.GetCurve<Quaternionf>(kRotationCurveName);
		Animation::Curve<Quaternionf>::Segument segment = curve.GetSegument(time); //!< 指定した時間における補間値を取得する

		//!< 補間値を計算する
		transform.rotation = Quaternionf::Slerp(segment.start, segment.end, static_cast<float>(segment.t));
	}

	{ //!< Scaleの取得
		const Animation::Curve<Vector3f>& curve = animation.GetCurve<Vector3f>(kScaleCurveName);
		Animation::Curve<Vector3f>::Segument segment = curve.GetSegument(time); //!< 指定した時間における補間値を取得する

		//!< 補間値を計算する
		transform.scale = Math::Lerp(segment.start, segment.end, static_cast<float>(segment.t));
	}

	return transform;
}

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationSequence class methods
////////////////////////////////////////////////////////////////////////////////////////////

const AnimationSequence::Description& AnimationSequence::GetDescription() const {
	Scheduler::TaskHandle handle = BaseAsset::GetTaskHandle();

	if (handle != Scheduler::TaskState::State::Completed) {
		STREAM_LOG_WARNING("Assets::AnimationSequence | AnimationSequence is not ready. name: {}, state: {}", GetName(), handle.GetState());
		handle.Wait(); //!< AnimationSequenceが準備できるまで待機
	}

	return description_;
}
