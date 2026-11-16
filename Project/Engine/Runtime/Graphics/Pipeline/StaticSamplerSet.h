#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Container/StringHashMap.h>

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

	void SetSamplerFilter(const std::string_view& name, SampleFilter filter, SampleMode mode, UINT anisotropic = 0);

	void SetSamplerLinear(const std::string_view& name, SampleMode mode);

	void SetSamplerPoint(const std::string_view& name, SampleMode mode);

	void SetSamplerAnisotropic(const std::string_view& name, SampleMode mode, UINT anisotropic);

	//* samplers set option *//

	bool Contains(const std::string_view& name) const;

	D3D12_STATIC_SAMPLER_DESC GetSampler(const std::string_view& name, ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0) const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	StringHashMap<D3D12_STATIC_SAMPLER_DESC> samplers;

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
