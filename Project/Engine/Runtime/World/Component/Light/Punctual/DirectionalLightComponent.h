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
// DirectionalLightComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class DirectionalLightComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// LightParameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct LightParameter {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Color3f color   = Color3f::White();
		float intensity = 1.0f; //!< [lux]

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
		float strength             = 1.0f; //!< [0.0 - 1.0]

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	DirectionalLightComponent(uint64_t handle, RefPtr<EntityBehaviour> owner) noexcept : BaseComponent(handle, owner) {}
	~DirectionalLightComponent() override = default;

	//* light parameter option *//

	void SetColor(const Color3f& color) { lightParameter_.color = color; }

	void SetIntensity(float intensity) { lightParameter_.intensity = intensity; }

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
