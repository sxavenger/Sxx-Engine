#include "PointLightComponent.h"
SXAVENGER_ENGINE_USING_(World)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Math/MathConstants.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [PointLightComponent] LightParameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

float PointLightComponent::LightParameter::ConvertScale(float value, LightingScale _scale) const {
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

float PointLightComponent::LightParameter::ConvertUnit(float value, LightUnit _unit) const {
	if (_unit == unit) {
		return value; //!< 同一単位の場合はそのまま返す
	}

	switch (_unit) {
		case LightUnit::Lumen:
			if (unit == LightUnit::Candela) return value * 4.0f * Math::kPi<float>; //!< candela -> lumen (lm = cd * 4π)
			break;

		case LightUnit::Candela:
			if (unit == LightUnit::Lumen) return value / (4.0f * Math::kPi<float>); //!< lumen -> candela (cd = lm / 4π)
			break;
	}

	return 0; //!< 変換できない場合は0を返す
}

float PointLightComponent::LightParameter::GetIntensity(LightUnit _unit, LightingScale _scale) const {
	return ConvertScale(ConvertUnit(intensity, _unit), _scale);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PointLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PointLightComponent::SetIntensity(float intensity, LightUnit unit, LightingScale scale) {
	lightParameter_.intensity = intensity;
	lightParameter_.unit      = unit;
	lightParameter_.scale     = scale;
}

float PointLightComponent::GetIntensity(LightUnit unit, LightingScale scale) const {
	return lightParameter_.GetIntensity(unit, scale);
}
