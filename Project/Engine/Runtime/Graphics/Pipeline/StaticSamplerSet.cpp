#include "StaticSamplerSet.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// StaticSamplerSet structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void StaticSamplerSet::SetSamplerFilter(const std::string& name, SampleFilter filter, SampleMode mode, UINT anisotropic) {
	D3D12_STATIC_SAMPLER_DESC desc = {};
	desc.Filter          = static_cast<D3D12_FILTER>(filter);
	desc.AddressU        = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	desc.AddressV        = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	desc.AddressW        = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	desc.MaxAnisotropy   = anisotropic;
	desc.ComparisonFunc  = D3D12_COMPARISON_FUNC_NEVER;
	desc.MaxLOD          = D3D12_FLOAT32_MAX;

	samplers[name] = desc;
}

void StaticSamplerSet::SetSamplerLinear(const std::string& name, SampleMode mode) {
	StaticSamplerSet::SetSamplerFilter(name, SampleFilter::Linear, mode, 0);
}

void StaticSamplerSet::SetSamplerPoint(const std::string& name, SampleMode mode) {
	StaticSamplerSet::SetSamplerFilter(name, SampleFilter::Point, mode, 0);
}

void StaticSamplerSet::SetSamplerAnisotropic(const std::string& name, SampleMode mode, UINT anisotropic) {
	StaticSamplerSet::SetSamplerFilter(name, SampleFilter::Anisotropic, mode, anisotropic);
}

bool StaticSamplerSet::Contains(const std::string& name) const {
	return samplers.contains(name);
}

D3D12_STATIC_SAMPLER_DESC StaticSamplerSet::GetSampler(const std::string& name, ShaderVisibility stage, UINT registerNumber, UINT registerSpace) const {
	StreamLogger::Assert(samplers.contains(name), std::format("sampler is not found. sampler name: {}", name));

	D3D12_STATIC_SAMPLER_DESC desc = samplers.at(name);
	desc.ShaderRegister   = registerNumber;
	desc.RegisterSpace    = registerSpace;
	desc.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(stage);

	return desc;
}
