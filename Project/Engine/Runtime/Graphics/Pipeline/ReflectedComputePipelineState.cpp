#include "ReflectedComputePipelineState.h"
SXAVENGER_ENGINE_USING_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// ReflectedComputePipelineState class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ReflectedComputePipelineState::BindShaderParameter(const GraphicsCommandContext& context, const ShaderParameter& parameter) const {
	layout_.BindComputeRootParameter(context, parameter);
}

ReflectedComputePipelineState ReflectedComputePipelineState::Create(const Device& device, const Desc& desc, D3D12_ROOT_SIGNATURE_FLAGS flags) {

	ReflectedComputePipelineState pipeline;

	//!< layoutの作成
	pipeline.layout_ = ReflectedComputePipelineState::CreateLayout(desc);

	//!< pipelineの作成
	ComputePipelineState::CreatePipelineState(pipeline, device, pipeline.layout_.CreateComputeRootSignature(device, flags), desc);

	return pipeline;
}

ReflectedComputePipelineState ReflectedComputePipelineState::Create(const Device& device, const Desc& desc, const StaticSamplerSet& samplers, D3D12_ROOT_SIGNATURE_FLAGS flags) {
	ReflectedComputePipelineState pipeline;

	//!< layoutの作成
	pipeline.layout_ = ReflectedComputePipelineState::CreateLayout(desc);

	//!< pipelineの作成
	ComputePipelineState::CreatePipelineState(pipeline, device, pipeline.layout_.CreateComputeRootSignature(device, samplers, flags), desc);

	return pipeline;
}

ShaderBindingLayout ReflectedComputePipelineState::CreateLayout(const Desc& desc) {

	ShaderBindingLayout layout;

	//!< ShaderBindingLayoutに反映
	Reflect(layout, ShaderVisibility::All, desc.GetShaderBlob());

	return layout;
}

void ReflectedComputePipelineState::Reflect(ShaderBindingLayout& layout, ShaderVisibility visibility, const ShaderBlob& blob) {
	StreamLogger::Assert(blob != nullptr, "required compute shader blob is not set."); 
	layout.Reflect(visibility, blob.Reflect());
}
