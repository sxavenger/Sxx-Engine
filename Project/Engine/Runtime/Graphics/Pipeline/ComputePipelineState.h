#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Core/Device.h"
#include "../Core/GraphicsCommandContext.h"
#include "../Shader/ShaderBlob.h"
#include "RootSignature.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// ComputePipelineState class
////////////////////////////////////////////////////////////////////////////////////////////
class ComputePipelineState final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Desc structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Desc {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* shader blob option *//

		void ResetShaderBlob();

		void SetShaderBlob(const ShaderBlob& blob);

		D3D12_SHADER_BYTECODE GetShaderBytecode() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* shader blob parameter *//

		ShaderBlob blob = nullptr; //!< Compute Shader Blob

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* pipeline state option *//

	void SetName(const std::wstring_view& name) const;
	void SetName(const std::string_view& name) const;

	void BindPipeline(const GraphicsCommandContext& context) const;

	//* operator [comparison] <std::nullptr_t> *//

	bool operator==(std::nullptr_t) const { return pipeline_ == nullptr; }
	bool operator!=(std::nullptr_t) const { return pipeline_ != nullptr; }

	//* static methods *//

	static ComputePipelineState Create(const Device& device, const RootSignature& rootSignature, const Desc& desc);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12PipelineState> pipeline_ = nullptr;

	//* parameter *//

	RootSignature rootSignature_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* create helper methods *//

	static ComPtr<ID3D12PipelineState> CreateComputePipelineState(RefPtr<ID3D12Device8> device, RefPtr<ID3D12RootSignature> rootSignature, const Desc& desc);

};

SXAVENGER_ENGINE_NAMESPACE_END
