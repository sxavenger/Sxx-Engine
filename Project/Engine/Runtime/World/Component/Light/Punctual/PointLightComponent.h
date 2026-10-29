#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../BaseComponent.h"
#include "../LightUnit.h"
#include "../ShadowFlags.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Color3.h>
#include <Lib/Flag/Flag.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// PointLightComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class PointLightComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// LightParameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct LightParameter {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		float GetIntensity(LightUnit _unit) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Color3f color   = Color3f::White();
		LightUnit unit  = LightUnit::Candela;
		float intensity = 1.0f;
		float radius    = 10.0f;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// ShadowParameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ShadowParameter {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		FlagEnum<ShadowFlags> flag = ShadowFlags::Default;
		float strength = 1.0f; //!< [0.0 - 1.0]

		// TODO: SourceAngleやカスケードの数などのパラメータを追加する

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	PointLightComponent(uint64_t handle, RefPtr<EntityBehaviour> owner) noexcept : BaseComponent(handle, owner) {}
	~PointLightComponent() override = default;

	//* light parameter option *//

	void SetColor(const Color3f& color) { lightParameter_.color = color; }

	const Color3f& GetColor() const { return lightParameter_.color; }

	void SetIntensity(float intensity, LightUnit unit = LightUnit::Candela);

	float GetIntensity(LightUnit unit = LightUnit::Candela) const;

	void SetRadius(float radius) { lightParameter_.radius = radius; }

	float GetRadius() const { return lightParameter_.radius; }

	void SetLightParameter(const LightParameter& parameter) { lightParameter_ = parameter; }

	const LightParameter& GetLightParameter() const { return lightParameter_; }

	//* shadow parameter option *//

	void SetCastShadow(bool castShadow) { shadowParameter_.flag.Set(ShadowFlags::CastShadow, castShadow); }

	void SetShadowParameter(const ShadowParameter& parameter) { shadowParameter_ = parameter; }

	const ShadowParameter& GetShadowParameter() const { return shadowParameter_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	LightParameter lightParameter_ = {};
	ShadowParameter shadowParameter_ = {};

};

SXAVENGER_ENGINE_NAMESPACE_END
