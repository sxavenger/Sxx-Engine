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
#include <Runtime/Assets/Handle/AssetHandle.h>
#include <Runtime/Assets/Texture/Texture.h>

//* lib
#include <Lib/Math/Color3.h>
#include <Lib/Flag/Flag.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(World)

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class SkyLightComponent final
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

		float ConvertScale(float value, LightingScale _scale) const;

		float GetIntensity(LightingScale _scale) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Color3f color       = Color3f::White();
		float intensity     = 1.0f; //!< [lux]
		LightingScale scale = LightingScale::Physical;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	SkyLightComponent(uint64_t handle, RefPtr<EntityBehaviour> owner) noexcept : BaseComponent(handle, owner) {}
	~SkyLightComponent() override = default;

	//* light parameter option *//

	void SetColor(const Color3f& color) { lightParameter_.color = color; }

	const Color3f& GetColor() const { return lightParameter_.color; }

	void SetIntensity(float intensity, LightingScale scale = LightingScale::Physical);

	float GetIntensity(LightingScale scale = LightingScale::Physical) const;

	void SetLightParameter(const LightParameter& parameter) { lightParameter_ = parameter; }

	const LightParameter& GetLightParameter() const { return lightParameter_; }

	//* texture option *//

	void SetEnvironment(const Assets::AssetHandle<Assets::Texture>& environment) { environment_ = environment; }

	const Assets::AssetHandle<Assets::Texture>& GetEnvironment() const { return environment_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	LightParameter lightParameter_ = {};

	Assets::AssetHandle<Assets::Texture> environment_ = std::nullopt;

};

SXAVENGER_ENGINE_NAMESPACE_END
