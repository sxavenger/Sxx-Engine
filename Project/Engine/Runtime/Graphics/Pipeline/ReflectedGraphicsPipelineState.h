#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Core/GraphicsCommandContext.h"
#include "GraphicsPipelineState.h"
#include "ShaderBindingLayout.h"
#include "StaticSamplerSet.h"
#include "ShaderParameter.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// ReflectedGraphicsPipelineState class
////////////////////////////////////////////////////////////////////////////////////////////
class ReflectedGraphicsPipelineState final
	: public GraphicsPipelineState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* layout option *//

	void BindShaderParameter(const GraphicsCommandContext& context, const ShaderParameter& parameter) const;

	//* static methods *//

	static ReflectedGraphicsPipelineState Create(
		const Device& device,
		const Desc& desc,
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED
	);

	static ReflectedGraphicsPipelineState Create(
		const Device& device,
		const Desc& desc,
		const StaticSamplerSet& samplers,
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED
	);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ShaderBindingLayout layout_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* create helper methods *//

	static ShaderBindingLayout CreateLayout(const Desc& desc);

	static void Reflect(ShaderBindingLayout& layout, ShaderVisibility visibility, const ShaderBlob& blob);

};

SXAVENGER_ENGINE_NAMESPACE_END
