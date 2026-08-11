#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Core/Device.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <vector>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// RootSignature class
////////////////////////////////////////////////////////////////////////////////////////////
class RootSignature final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Desc structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Desc {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* desc option *//

		void Reset();

		//* parameter option *//

		void AppendVirtualAddress(ShaderVisibility stage, D3D12_ROOT_PARAMETER_TYPE type, UINT registerNumber, UINT registerSpace = 0);
		void SetVirtualAddress(uint32_t index, ShaderVisibility stage, D3D12_ROOT_PARAMETER_TYPE type, UINT registerNumber, UINT registerSpace = 0);

		void AppendDescriptorHandle(ShaderVisibility stage, D3D12_DESCRIPTOR_RANGE_TYPE type, UINT registerNumber, UINT registerSpace = 0);
		void SetDescriptorHandle(uint32_t index, ShaderVisibility stage, D3D12_DESCRIPTOR_RANGE_TYPE type, UINT registerNumber, UINT registerSpace = 0);

		void Append32bitConstants(ShaderVisibility stage, UINT num32bit, UINT registerNumber, UINT registerSpace = 0);
		void Set32bitConstants(uint32_t index, ShaderVisibility stage, UINT num32bit, UINT registerNumber, UINT registerSpace = 0);

		//* sampler option *//

		void AppendSamplerDesc(const D3D12_STATIC_SAMPLER_DESC& desc);

		void AppendSamplerFilter(SampleFilter filter, SampleMode mode, ShaderVisibility stage, UINT anisotropic, UINT registerNumber, UINT registerSpace = 0);

		void AppendSamplerLinear(SampleMode mode, ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);

		void AppendSamplerPoint(SampleMode mode, ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);

		void AppendSamplerAnisotropic(SampleMode mode, ShaderVisibility stage, UINT registerNumber, UINT anisotropic, UINT registerSpace = 0);

		//* root signature option *//

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC CreateDesc(D3D12_ROOT_SIGNATURE_FLAGS flags) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::vector<D3D12_ROOT_PARAMETER1> parameters;
		std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;

		std::list<D3D12_DESCRIPTOR_RANGE1> ranges;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GraphicsDesc structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct GraphicsDesc
		: public Desc {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* virtual address option *//

		void AppendVirtualAddressCBV(ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);
		void SetVirtualAddressCBV(uint32_t index, ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);

		void AppendVirtualAddressSRV(ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);
		void SetVirtualAddressSRV(uint32_t index, ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);

		void AppendVirtualAddressUAV(ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);
		void SetVirtualAddressUAV(uint32_t index, ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);

		//* descriptor handle option *//

		void AppendDescriptorHandleCBV(ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);
		void SetDescriptorHandleCBV(uint32_t index, ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);

		void AppendDescriptorHandleSRV(ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);
		void SetDescriptorHandleSRV(uint32_t index, ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);

		void AppendDescriptorHandleUAV(ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);
		void SetDescriptorHandleUAV(uint32_t index, ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);

		//* sampler option *//

		void AppendSamplerLinear(SampleMode mode, ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);

		void AppendSamplerPoint(SampleMode mode, ShaderVisibility stage, UINT registerNumber, UINT registerSpace = 0);

		void AppendSamplerAnisotropic(SampleMode mode, ShaderVisibility stage, UINT registerNumber, UINT anisotropic, UINT registerSpace = 0);

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// ComputeDesc structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ComputeDesc
		: public Desc {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* virtual address option *//

		void AppendVirtualAddressCBV(UINT registerNumber, UINT registerSpace = 0);
		void SetVirtualAddressCBV(uint32_t index, UINT registerNumber, UINT registerSpace = 0);

		void AppendVirtualAddressSRV(UINT registerNumber, UINT registerSpace = 0);
		void SetVirtualAddressSRV(uint32_t index, UINT registerNumber, UINT registerSpace = 0);

		void AppendVirtualAddressUAV(UINT registerNumber, UINT registerSpace = 0);
		void SetVirtualAddressUAV(uint32_t index, UINT registerNumber, UINT registerSpace = 0);

		//* descriptor handle option *//

		void AppendDescriptorHandleCBV(UINT registerNumber, UINT registerSpace = 0);
		void SetDescriptorHandleCBV(uint32_t index, UINT registerNumber, UINT registerSpace = 0);

		void AppendDescriptorHandleSRV(UINT registerNumber, UINT registerSpace = 0);
		void SetDescriptorHandleSRV(uint32_t index, UINT registerNumber, UINT registerSpace = 0);

		void AppendDescriptorHandleUAV(UINT registerNumber, UINT registerSpace = 0);
		void SetDescriptorHandleUAV(uint32_t index, UINT registerNumber, UINT registerSpace = 0);

		//* sampler option *//

		void AppendSamplerLinear(SampleMode mode, UINT registerNumber, UINT registerSpace = 0);

		void AppendSamplerPoint(SampleMode mode, UINT registerNumber, UINT registerSpace = 0);

		void AppendSamplerAnisotropic(SampleMode mode, UINT registerNumber, UINT anisotropic, UINT registerSpace = 0);

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	RootSignature() = default;
	RootSignature(const ComPtr<ID3D12RootSignature>& rootSignature) : rootSignature_(rootSignature) {}

	//* root signature option *//

	static RootSignature Create(
		const Device& device,
		const Desc& desc,
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED
	);

	RefPtr<ID3D12RootSignature> Get() const { return rootSignature_.Get(); }

	//* operator [comparison] <std::nullptr_t> *//

	bool operator==(std::nullptr_t) const { return rootSignature_ == nullptr; }
	bool operator!=(std::nullptr_t) const { return rootSignature_ != nullptr; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12RootSignature> rootSignature_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* create helper methods *//

	static ComPtr<ID3D12RootSignature> CreateRootSignature(RefPtr<ID3D12Device8> device, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& desc);

};

SXAVENGER_ENGINE_NAMESPACE_END
