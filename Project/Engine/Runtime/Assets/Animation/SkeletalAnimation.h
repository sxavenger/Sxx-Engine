#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/BaseAsset.h"
#include "Animation.h"
#include "SkeletalAnimationMetadata.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Time/TimeUnit.h>

//* c++
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// SkeletalAnimation class
////////////////////////////////////////////////////////////////////////////////////////////
class SkeletalAnimation final
	: public BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Description structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Description {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//=========================================================================================
		// private variables
		//=========================================================================================

		TimePointd<TimeUnit::Second> duration; //!< アニメーション全体時間
		std::unordered_map<std::string, Animation> boneAnimations; //!< bone名をキーにして, Animationを保持するmap

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	SkeletalAnimation(const BaseAssetMetadata& metadata) noexcept : BaseAsset(metadata) {}
	~SkeletalAnimation() noexcept override = default;

	//* description option *//

	const Description& GetDescription() const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* directory *//

	static inline const std::filesystem::path kDirectory = L"Animations"; //!< SkeletalAnimationの保存ディレクトリ

	static inline const std::string_view kTranslationCurveName = "translation"; //!< translationのcurve名
	static inline const std::string_view kRotationCurveName    = "rotation";    //!< rotationのcurve名
	static inline const std::string_view kScaleCurveName       = "scale";       //!< scaleのcurve名

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* metadata *//

	SkeletalAnimationMetadata metadata_;

	//* description *//

	Description description_;

	//-----------------------------------------------------------------------------------------
	// friend class
	//-----------------------------------------------------------------------------------------
	friend class SkeletalAnimationBuilder;

};

SXAVENGER_ENGINE_NAMESPACE_END
