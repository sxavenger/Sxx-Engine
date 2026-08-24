#include "ReflectedGraphicsPipelineState.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ReflectedGraphicsPipelineState class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ReflectedGraphicsPipelineState::BindShaderParameter(const GraphicsCommandContext& context, const ShaderParameter& parameter) const {
	layout_.BindGraphicsRootParameter(context, parameter);
}

ReflectedGraphicsPipelineState ReflectedGraphicsPipelineState::Create(const Device& device, const Desc& desc, D3D12_ROOT_SIGNATURE_FLAGS flags) {

	ReflectedGraphicsPipelineState pipeline;

	//!< layoutの作成
	pipeline.layout_ = ReflectedGraphicsPipelineState::CreateLayout(desc);

	//!< pipelineの作成
	GraphicsPipelineState::CreatePipelineState(pipeline, device, pipeline.layout_.CreateGraphicsRootSignature(device, flags), desc);

	return pipeline;
}

ReflectedGraphicsPipelineState ReflectedGraphicsPipelineState::Create(const Device& device, const Desc& desc, const StaticSamplerSet& samplers, D3D12_ROOT_SIGNATURE_FLAGS flags) {

	ReflectedGraphicsPipelineState pipeline;

	//!< layoutの作成
	pipeline.layout_ = ReflectedGraphicsPipelineState::CreateLayout(desc);

	//!< pipelineの作成
	GraphicsPipelineState::CreatePipelineState(pipeline, device, pipeline.layout_.CreateGraphicsRootSignature(device, samplers, flags), desc);

	return pipeline;
}

ShaderBindingLayout ReflectedGraphicsPipelineState::CreateLayout(const Desc& desc) {

	ShaderBindingLayout layout;

	//!< ShaderBindingLayoutに反映
	switch (desc.type) {
		case GraphicsType::Vertex:
			Reflect(layout, ShaderVisibility::Vertex,   desc.GetShaderBlob(CompileProfile::Vertex));
			Reflect(layout, ShaderVisibility::Hull,     desc.GetShaderBlob(CompileProfile::Hull));
			Reflect(layout, ShaderVisibility::Domain,   desc.GetShaderBlob(CompileProfile::Domain));
			Reflect(layout, ShaderVisibility::Geometry, desc.GetShaderBlob(CompileProfile::Geometry));
			Reflect(layout, ShaderVisibility::Pixel,    desc.GetShaderBlob(CompileProfile::Pixel));
			break;

		case GraphicsType::Mesh:
			Reflect(layout, ShaderVisibility::Amplification, desc.GetShaderBlob(CompileProfile::Amplification));
			Reflect(layout, ShaderVisibility::Mesh,          desc.GetShaderBlob(CompileProfile::Mesh));
			Reflect(layout, ShaderVisibility::Pixel,         desc.GetShaderBlob(CompileProfile::Pixel));
			break;
	}

	return layout;
}

void ReflectedGraphicsPipelineState::Reflect(ShaderBindingLayout& layout, ShaderVisibility visibility, const ShaderBlob& blob) {
	if (blob == nullptr) {
		STREAM_ASSERT(
			visibility != ShaderVisibility::Pixel && visibility != ShaderVisibility::Mesh && visibility != ShaderVisibility::Vertex,
			"required graphics shader blob is not set. visibility: {}", visibility
		); //!< Pixel, Mesh, Vertexのいずれかのvisibilityの場合は必須なのでエラー
		return;
	}

	layout.Reflect(visibility, blob.Reflect());
}
