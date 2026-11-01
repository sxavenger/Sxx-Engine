#include "DirectionalLightComponent.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Math/MathConstants.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [DirectionalLightComponent] LightParameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

float DirectionalLightComponent::LightParameter::ConvertScale(float value, LightingScale _scale) const {
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

float DirectionalLightComponent::LightParameter::GetIntensity(LightingScale _scale) const {
	return ConvertScale(intensity, _scale);
}

////////////////////////////////////////////////////////////////////////////////////////////
// DirectionalLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectionalLightComponent::SetIntensity(float intensity, LightingScale scale) {
	lightParameter_.intensity = intensity;
	lightParameter_.scale     = scale;
}

float DirectionalLightComponent::GetIntensity(LightingScale scale) const {
	return lightParameter_.GetIntensity(scale);
}
