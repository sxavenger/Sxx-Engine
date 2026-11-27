#include "SkyLightComponent.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Math/MathConstants.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [SkyLightComponent] LightParameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

float SkyLightComponent::LightParameter::ConvertScale(float value, LightingScale _scale) const {
	if (_scale == scale) {
		return value; //!< 同一スケールの場合はそのまま返す
	}

	switch (_scale) {
		case LightingScale::Physical:
			if (scale == LightingScale::Lambert) return value * Math::kPi<float>; //!< Lambert -> Physical (Physical = Lambert * π)
			break;

		case LightingScale::Lambert:
			if (scale == LightingScale::Physical) return value / Math::kPi<float>; //!< Physical -> Lambert (Lambert = Physical / π)
			break;
	}

	return 0; //!< 変換できない場合は0を返す
}

float SkyLightComponent::LightParameter::GetIntensity(LightingScale _scale) const {
	return ConvertScale(intensity, _scale);
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyLightComponent::SetIntensity(float intensity, LightingScale scale) {
	lightParameter_.intensity = intensity;
	lightParameter_.scale     = scale;
}

float SkyLightComponent::GetIntensity(LightingScale scale) const {
	return lightParameter_.GetIntensity(scale);
}
