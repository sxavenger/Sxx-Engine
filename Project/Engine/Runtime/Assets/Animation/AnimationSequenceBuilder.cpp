#include "AnimationSequenceBuilder.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Assimp/AssimpCommon.h"
#include "../Assimp/AssimpImporter.h"
#include "../Assimp/AssimpAnimation.h"
#include "../Assimp/AssimpNodeAnimation.h"

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonFile.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationSequenceBuilder class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AnimationSequenceBuilder::Build(std::shared_ptr<AnimationSequence>& animation) {
	//!< ファイルパスの取得
	const std::filesystem::path& filepath = animation->GetFilepath();

	if (!std::filesystem::exists(filepath)) {
		STREAM_LOG_ERROR("Assets::AnimationSequenceBuilder | file does not exist. filepath: {}", filepath.generic_string());
		return; //!< ファイルが存在しない場合は処理を終了
	}

	//!< metadataの読み込み
	json::node node = JsonFile::Load(filepath);

	//!< metadataの設定
	const AnimationSequenceMetadata& metadata = animation->metadata_ = AnimationSequenceMetadata::Deserialize(node["metadata"]);

	switch (metadata.GetType()) {
		case MetadataType::Reference:
			animation->description_ = AnimationSequenceBuilder::BuildReference(animation->GetDirectory(), metadata.GetReferenceData());
			break;

		case MetadataType::Inline:
			animation->description_ = {}; //!< TODO: Inline情報の定義.
			STREAM_LOG_ERROR("Assets::AnimationSequenceBuilder | inline animation sequence metadata type is not defined. filepath: {}", filepath.generic_string());
			return;

		default:
			STREAM_LOG_ERROR("Assets::AnimationSequenceBuilder | animation sequence metadata type is unknown. filepath: {}", filepath.generic_string());
			return; //!< metadataがUnknownの場合は処理を終了
	}

	STREAM_LOG_INFO("Assets::AnimationSequenceBuilder | animation sequence build completed. name: {}", animation->GetName());
}

AnimationSequence::Description AnimationSequenceBuilder::BuildReference(const std::filesystem::path& directory, const AnimationSequenceMetadata::ReferenceData& data) {

	//!< 参照されているanimationのファイルパスの取得
	std::filesystem::path path = (directory / data.filepath).lexically_normal();

	//!< animationの取得
	const AssimpImporter importer   = AssimpCommon::GetImporter(path);
	const AssimpAnimation animation = importer.GetAnimation(data.index);

	//!< descriptionの作成
	AnimationSequence::Description description;

	//!< animationの全体時間の設定
	description.duration = animation.GetDuration();

	
	for (uint32_t channel = 0; channel < animation.GetChannelCount(); ++channel) {

		const AssimpNodeAnimation& nodeAnimation = animation.GetChannel(channel); //!< nodeAnimationの取得
		Animation& boneAnimation                 = description.boneAnimations[nodeAnimation.GetNodeName()]; //!< boneAnimationの取得
		
		{ //!< Positionの設定

			Animation::Curve<Vector3f> curve;
			for (uint32_t i = 0; i < nodeAnimation.GetPositionKeyCount(); ++i) {
				AssimpNodeAnimation::Key<Vector3f> key = nodeAnimation.GetPositionKey(i, animation.GetTicksPerSecond());
				curve.keyframes.emplace_back(key.time, key.value);
			}

			Animation::Curve<Vector3f>::Sort(curve); //!< キーフレームを時刻順にソートする
			boneAnimation.SetCurve(AnimationSequence::Description::kTranslationCurveName, std::move(curve));
		}

		{ //!< Rotationの設定

			Animation::Curve<Quaternionf> curve;
			for (uint32_t i = 0; i < nodeAnimation.GetRotationKeyCount(); ++i) {
				AssimpNodeAnimation::Key<Quaternionf> key = nodeAnimation.GetRotationKey(i, animation.GetTicksPerSecond());
				curve.keyframes.emplace_back(key.time, key.value);
			}

			Animation::Curve<Quaternionf>::Sort(curve); //!< キーフレームを時刻順にソートする
			boneAnimation.SetCurve(AnimationSequence::Description::kRotationCurveName, std::move(curve));
		}

		{ //!< Scaleの設定

			Animation::Curve<Vector3f> curve;
			for (uint32_t i = 0; i < nodeAnimation.GetScaleKeyCount(); ++i) {
				AssimpNodeAnimation::Key<Vector3f> key = nodeAnimation.GetScaleKey(i, animation.GetTicksPerSecond());
				curve.keyframes.emplace_back(key.time, key.value);
			}

			Animation::Curve<Vector3f>::Sort(curve); //!< キーフレームを時刻順にソートする
			boneAnimation.SetCurve(AnimationSequence::Description::kScaleCurveName, std::move(curve));
		}
	}

	return description;
}
