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

float PointLightComponent::LightParameter::GetIntensity(LightUnit _unit) const {
	if (_unit == unit) {
		return intensity; //!< 同一単位の場合はそのまま返す
	}

	switch (_unit) {
		case LightUnit::Lumen:
			if (unit == LightUnit::Candela) {
				return intensity * 4.0f * Math::kPi<float>; //!< candera -> lumen (lm = cd * 4π)
			}
			break;

		case LightUnit::Candela:
			if (unit == LightUnit::Lumen) {
				return intensity / (4.0f * Math::kPi<float>); //!< lumen -> candela (cd = lm / 4π)
			}
			break;
	}

	return 0; //!< 変換できない場合は0を返す
}

////////////////////////////////////////////////////////////////////////////////////////////
// PointLightComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PointLightComponent::SetIntensity(float intensity, LightUnit unit) {
	lightParameter_.intensity = intensity;
	lightParameter_.unit      = unit;
}

float PointLightComponent::GetIntensity(LightUnit unit) const {
	return lightParameter_.GetIntensity(unit);
}
