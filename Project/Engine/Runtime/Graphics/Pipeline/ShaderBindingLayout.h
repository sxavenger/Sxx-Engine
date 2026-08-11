#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Core/Device.h"
#include "../Core/GraphicsCommandContext.h"
#include "../Shader/ShaderReflection.h"
#include "ShaderParameter.h"
#include "ShaderBindingSlot.h"
#include "RootSignature.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <vector>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBindingLayout class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderBindingLayout final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Reflect(ShaderVisibility visibility, const ShaderReflection& reflection);

	//* slot options *//

	void Reset();

	bool Contains(const std::string& name) const;

	ShaderBindingSlot& GetSlot(const std::string& name);
	const ShaderBindingSlot& GetSlot(const std::string& name) const;

	//* root signature option *//

	RootSignature CreateGraphicsRootSignature(
		const Device& device,
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED
	);

	RootSignature CreateComputeRootSignature(
		const Device& device,
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED
	);

	// TODO: StaticSamplerStateの指定Descで作成する.

	//* bind option *//

	void BindGraphicsRootParameter(const GraphicsCommandContext& context, const ShaderParameter& parameter);

	void BindComputeRootParameter(const GraphicsCommandContext& context, const ShaderParameter& parameter);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<std::string, ShaderBindingSlot> slots_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Insert(ShaderVisibility visibility, const D3D12_SHADER_INPUT_BIND_DESC& desc, const ShaderReflection& reflection);

};

SXAVENGER_ENGINE_NAMESPACE_END
