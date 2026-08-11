#include "RootSignature.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/EnumUtil.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [RootSignature] Desc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void RootSignature::Desc::Reset() {
	parameters.clear();
	samplers.clear();
	ranges.clear();
}

void RootSignature::Desc::AppendVirtualAddress(ShaderVisibility stage, D3D12_ROOT_PARAMETER_TYPE type, UINT shaderRegister, UINT registerSpace) {
	D3D12_ROOT_PARAMETER1 parameter = {};
	parameter.ParameterType             = type;
	parameter.ShaderVisibility          = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	parameter.Descriptor.ShaderRegister = shaderRegister;
	parameter.Descriptor.RegisterSpace  = registerSpace;

	parameters.emplace_back(parameter);
}

void RootSignature::Desc::SetVirtualAddress(uint32_t index, ShaderVisibility stage, D3D12_ROOT_PARAMETER_TYPE type, UINT shaderRegister, UINT registerSpace) {
	D3D12_ROOT_PARAMETER1 parameter = {};
	parameter.ParameterType             = type;
	parameter.ShaderVisibility          = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	parameter.Descriptor.ShaderRegister = shaderRegister;
	parameter.Descriptor.RegisterSpace  = registerSpace;

	if (index >= parameters.size()) {
		parameters.resize(index + 1); //!< indexがparametersのサイズ以上の場合はparametersをリサイズする.
	}

	parameters[index] = parameter; //!< indexの位置にparameterを設定する.
}

void RootSignature::Desc::AppendDescriptorHandle(ShaderVisibility stage, D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT registerSpace) {
	D3D12_DESCRIPTOR_RANGE1 range = {};
	range.BaseShaderRegister                = shaderRegister;
	range.RegisterSpace                     = registerSpace;
	range.NumDescriptors                    = 1;
	range.RangeType                         = type;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	ranges.emplace_back(range);

	D3D12_ROOT_PARAMETER1 parameter = {};
	parameter.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter.ShaderVisibility                    = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	parameter.DescriptorTable.pDescriptorRanges   = &ranges.back();
	parameter.DescriptorTable.NumDescriptorRanges = 1;

	parameters.emplace_back(parameter);
}

void RootSignature::Desc::SetDescriptorHandle(uint32_t index, ShaderVisibility stage, D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT registerSpace) {
	D3D12_DESCRIPTOR_RANGE1 range = {};
	range.BaseShaderRegister                = shaderRegister;
	range.RegisterSpace                     = registerSpace;
	range.NumDescriptors                    = 1;
	range.RangeType                         = type;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	ranges.emplace_back(range);

	D3D12_ROOT_PARAMETER1 parameter = {};
	parameter.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter.ShaderVisibility                    = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	parameter.DescriptorTable.pDescriptorRanges   = &ranges.back();
	parameter.DescriptorTable.NumDescriptorRanges = 1;

	if (index >= parameters.size()) {
		parameters.resize(index + 1); //!< indexがparametersのサイズ以上の場合はparametersをリサイズする.
	}

	parameters[index] = parameter;
}

void RootSignature::Desc::Append32bitConstants(ShaderVisibility stage, UINT num32bit, UINT shaderRegister, UINT registerSpace) {
	D3D12_ROOT_PARAMETER1 parameter = {};
	parameter.ParameterType            = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	parameter.ShaderVisibility         = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	parameter.Constants.Num32BitValues = num32bit;
	parameter.Constants.ShaderRegister = shaderRegister;
	parameter.Constants.RegisterSpace  = registerSpace;

	parameters.emplace_back(parameter);
}

void RootSignature::Desc::Set32bitConstants(uint32_t index, ShaderVisibility stage, UINT num32bit, UINT shaderRegister, UINT registerSpace) {
	D3D12_ROOT_PARAMETER1 parameter = {};
	parameter.ParameterType            = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	parameter.ShaderVisibility         = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	parameter.Constants.Num32BitValues = num32bit;
	parameter.Constants.ShaderRegister = shaderRegister;
	parameter.Constants.RegisterSpace  = registerSpace;

	if (index >= parameters.size()) {
		parameters.resize(index + 1); //!< indexがparametersのサイズ以上の場合はparametersをリサイズする.
	}

	parameters[index] = parameter;
}

void RootSignature::Desc::AppendSamplerFilter(SampleFilter filter, SampleMode mode, ShaderVisibility stage, uint32_t anisotropic, UINT shaderRegister, UINT registerSpace) {
	D3D12_STATIC_SAMPLER_DESC desc = {};
	desc.Filter           = static_cast<D3D12_FILTER>(filter);
	desc.MaxAnisotropy    = anisotropic; //!< 異方性フィルタリングパラメーター(typeがAnisotropicのときのみ有効)
	desc.AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	desc.AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	desc.AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	desc.ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	desc.MaxLOD           = D3D12_FLOAT32_MAX;
	desc.ShaderRegister   = shaderRegister;
	desc.RegisterSpace    = registerSpace;
	desc.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(stage);

	samplers.emplace_back(desc);
}

void RootSignature::Desc::AppendSamplerLinear(SampleMode mode, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	AppendSamplerFilter(SampleFilter::Linear, mode, stage, 0, shaderRegister, registerSpace);
}

void RootSignature::Desc::AppendSamplerPoint(SampleMode mode, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	AppendSamplerFilter(SampleFilter::Point, mode, stage, 0, shaderRegister, registerSpace);
}

void RootSignature::Desc::AppendSamplerAnisotropic(SampleMode mode, ShaderVisibility stage, UINT shaderRegister, uint32_t anisotropic, UINT registerSpace) {
	AppendSamplerFilter(SampleFilter::Anisotropic, mode, stage, anisotropic, shaderRegister, registerSpace);
}

D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSignature::Desc::CreateDesc(D3D12_ROOT_SIGNATURE_FLAGS flags) const {

	D3D12_ROOT_SIGNATURE_DESC1 desc = {};
	desc.Flags = flags;

	if (!parameters.empty()) {
		desc.NumParameters = static_cast<UINT>(parameters.size());
		desc.pParameters   = parameters.data();
	}

	if (!samplers.empty()) {
		desc.NumStaticSamplers = static_cast<UINT>(samplers.size());
		desc.pStaticSamplers   = samplers.data();
	}

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC version = {};
	version.Version  = D3D_ROOT_SIGNATURE_VERSION_1_1;
	version.Desc_1_1 = desc;

	return version;
}

////////////////////////////////////////////////////////////////////////////////////////////
// [RootSignature] GraphicsDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void RootSignature::GraphicsDesc::AppendVirtualAddressCBV(ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::AppendVirtualAddress(stage, D3D12_ROOT_PARAMETER_TYPE_CBV, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::SetVirtualAddressCBV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::SetVirtualAddress(index, stage, D3D12_ROOT_PARAMETER_TYPE_CBV, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::AppendVirtualAddressSRV(ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::AppendVirtualAddress(stage, D3D12_ROOT_PARAMETER_TYPE_SRV, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::SetVirtualAddressSRV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::SetVirtualAddress(index, stage, D3D12_ROOT_PARAMETER_TYPE_SRV, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::AppendVirtualAddressUAV(ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::AppendVirtualAddress(stage, D3D12_ROOT_PARAMETER_TYPE_UAV, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::SetVirtualAddressUAV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::SetVirtualAddress(index, stage, D3D12_ROOT_PARAMETER_TYPE_UAV, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::AppendDescriptorHandleCBV(ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::AppendDescriptorHandle(stage, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::SetDescriptorHandleCBV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::SetDescriptorHandle(index, stage, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::AppendDescriptorHandleSRV(ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::AppendDescriptorHandle(stage, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::SetDescriptorHandleSRV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::SetDescriptorHandle(index, stage, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::AppendDescriptorHandleUAV(ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::AppendDescriptorHandle(stage, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::SetDescriptorHandleUAV(uint32_t index, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::SetDescriptorHandle(index, stage, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::AppendSamplerLinear(SampleMode mode, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::AppendSamplerLinear(mode, stage, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::AppendSamplerPoint(SampleMode mode, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) {
	Desc::AppendSamplerPoint(mode, stage, shaderRegister, registerSpace);
}

void RootSignature::GraphicsDesc::AppendSamplerAnisotropic(SampleMode mode, ShaderVisibility stage, UINT shaderRegister, uint32_t anisotropic, UINT registerSpace) {
	Desc::AppendSamplerAnisotropic(mode, stage, shaderRegister, anisotropic, registerSpace);
}

////////////////////////////////////////////////////////////////////////////////////////////
// [RootSignature] ComputeDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void RootSignature::ComputeDesc::AppendVirtualAddressCBV(UINT shaderRegister, UINT registerSpace) {
	Desc::AppendVirtualAddress(ShaderVisibility::All, D3D12_ROOT_PARAMETER_TYPE_CBV, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::SetVirtualAddressCBV(uint32_t index, UINT shaderRegister, UINT registerSpace) {
	Desc::SetVirtualAddress(index, ShaderVisibility::All, D3D12_ROOT_PARAMETER_TYPE_CBV, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::AppendVirtualAddressSRV(UINT shaderRegister, UINT registerSpace) {
	Desc::AppendVirtualAddress(ShaderVisibility::All, D3D12_ROOT_PARAMETER_TYPE_SRV, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::SetVirtualAddressSRV(uint32_t index, UINT shaderRegister, UINT registerSpace) {
	Desc::SetVirtualAddress(index, ShaderVisibility::All, D3D12_ROOT_PARAMETER_TYPE_SRV, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::AppendVirtualAddressUAV(UINT shaderRegister, UINT registerSpace) {
	Desc::AppendVirtualAddress(ShaderVisibility::All, D3D12_ROOT_PARAMETER_TYPE_UAV, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::SetVirtualAddressUAV(uint32_t index, UINT shaderRegister, UINT registerSpace) {
	Desc::SetVirtualAddress(index, ShaderVisibility::All, D3D12_ROOT_PARAMETER_TYPE_UAV, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::AppendDescriptorHandleCBV(UINT shaderRegister, UINT registerSpace) {
	Desc::AppendDescriptorHandle(ShaderVisibility::All, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::SetDescriptorHandleCBV(uint32_t index, UINT shaderRegister, UINT registerSpace) {
	Desc::SetDescriptorHandle(index, ShaderVisibility::All, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::AppendDescriptorHandleSRV(UINT shaderRegister, UINT registerSpace) {
	Desc::AppendDescriptorHandle(ShaderVisibility::All, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::SetDescriptorHandleSRV(uint32_t index, UINT shaderRegister, UINT registerSpace) {
	Desc::SetDescriptorHandle(index, ShaderVisibility::All, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::AppendDescriptorHandleUAV(UINT shaderRegister, UINT registerSpace) {
	Desc::AppendDescriptorHandle(ShaderVisibility::All, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::SetDescriptorHandleUAV(uint32_t index, UINT shaderRegister, UINT registerSpace) {
	Desc::SetDescriptorHandle(index, ShaderVisibility::All, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::AppendSamplerLinear(SampleMode mode, UINT shaderRegister, UINT registerSpace) {
	Desc::AppendSamplerLinear(mode, ShaderVisibility::All, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::AppendSamplerPoint(SampleMode mode, UINT shaderRegister, UINT registerSpace) {
	Desc::AppendSamplerPoint(mode, ShaderVisibility::All, shaderRegister, registerSpace);
}

void RootSignature::ComputeDesc::AppendSamplerAnisotropic(SampleMode mode, UINT shaderRegister, uint32_t anisotropic, UINT registerSpace) {
	Desc::AppendSamplerAnisotropic(mode, ShaderVisibility::All, shaderRegister, anisotropic, registerSpace);
}

////////////////////////////////////////////////////////////////////////////////////////////
// RootSignature class methods
////////////////////////////////////////////////////////////////////////////////////////////

RootSignature RootSignature::Create(
	const Device& device,
	const Desc& desc,
	D3D12_ROOT_SIGNATURE_FLAGS flags) {

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC version = desc.CreateDesc(flags);
	return RootSignature::CreateRootSignature(device.GetDevice(), version);
}

ComPtr<ID3D12RootSignature> RootSignature::CreateRootSignature(RefPtr<ID3D12Device8> device, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& desc) {
	
	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> error;

	auto hr = D3D12SerializeVersionedRootSignature(&desc, &blob, &error);
	if (FAILED(hr)) {
		std::string_view message(reinterpret_cast<const char*>(error->GetBufferPointer()), error->GetBufferSize());
		StreamLogger::Exception("root signature create error.", message);
	}

	ComPtr<ID3D12RootSignature> rootSignature;

	hr = device->CreateRootSignature(
		0,
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		IID_PPV_ARGS(rootSignature.GetAddressOf())
	);
	ComPtrUtil::Assert(hr, L"root signature create error.");
	
	return rootSignature;
}
