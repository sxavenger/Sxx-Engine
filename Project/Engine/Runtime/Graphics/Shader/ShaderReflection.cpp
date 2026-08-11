#include "ShaderReflection.h"
SXAVENGER_ENGINE_USING_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderReflection class methods
////////////////////////////////////////////////////////////////////////////////////////////

D3D12_SHADER_DESC ShaderReflection::GetShaderDesc() const {
	D3D12_SHADER_DESC desc = {};
	auto hr = reflection_->GetDesc(&desc);
	ComPtrUtil::Assert(hr, L"shader reflection get desc failed.");

	return desc;
}

D3D12_SHADER_INPUT_BIND_DESC ShaderReflection::GetResourceBindingDesc(UINT index) const {
	D3D12_SHADER_INPUT_BIND_DESC desc = {};
	auto hr = reflection_->GetResourceBindingDesc(index, &desc);
	ComPtrUtil::Assert(hr, L"shader reflection get resource binding desc failed.");

	return desc;
}

ID3D12ShaderReflectionConstantBuffer* ShaderReflection::GetConstantBufferByName(const std::string_view& name) const {
	return reflection_->GetConstantBufferByName(name.data());
}
