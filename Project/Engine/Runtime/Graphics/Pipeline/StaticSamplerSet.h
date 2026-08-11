#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <string>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// StaticSamplerSet structure
////////////////////////////////////////////////////////////////////////////////////////////
struct StaticSamplerSet final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* sampler option *//

	void SetSamplerFilter(const std::string& name, SampleFilter filter, SampleMode mode, UINT anisotropic);

	void SetSamplerLinear(const std::string& name, SampleMode mode);

	void SetSamplerPoint(const std::string& name, SampleMode mode);

	void SetSamplerAnisotropic(const std::string& name, SampleMode mode, UINT anisotropic);

	//* samplers set option *//

	bool Contains(const std::string& name) const;

	D3D12_STATIC_SAMPLER_DESC GetSampler(const std::string& name, ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0) const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::unordered_map<std::string, D3D12_STATIC_SAMPLER_DESC> samplers;

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
