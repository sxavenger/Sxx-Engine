#include "ShaderBindingSlot.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [ShaderBindingSlot] Declaration structure methods
////////////////////////////////////////////////////////////////////////////////////////////

ShaderBindingSlot::BindingType ShaderBindingSlot::Declaration::GetBindingType() const {
	switch (type) {
		case ShaderInputType::ConstantBuffer:
			return Declaration::IsRootConstants(type, name) ? BindingType::RootConstants : BindingType::VirtualAddressCBV;

		case ShaderInputType::StructuredBuffer:
		case ShaderInputType::AccelerationStructure:
			return BindingType::VirtualAddressSRV;

		case ShaderInputType::RWStructuredBuffer:
			return BindingType::VirtualAddressUAV;

		case ShaderInputType::Texture:
			return BindingType::HandleSRV;

		case ShaderInputType::RWTexture:
		case ShaderInputType::RWStructuredBufferWithCounter:
			return BindingType::HandleUAV;

		case ShaderInputType::Sampler:
			return BindingType::Sampler;

		default:
			STREAM_EXCEPTION("ShaderInputType is undefined.");
	}
}

ShaderBindingSlot::Declaration ShaderBindingSlot::Declaration::Create(const D3D12_SHADER_INPUT_BIND_DESC& desc, const ShaderReflection& reflection) {
	Declaration declaration = {};
	declaration.name           = desc.Name;
	declaration.type           = static_cast<ShaderInputType>(desc.Type);
	declaration.registerNumber = desc.BindPoint;
	declaration.registerSpace  = desc.Space;

	if (Declaration::IsRootConstants(declaration.type, declaration.name)) {
		ID3D12ShaderReflectionConstantBuffer* constant = reflection.GetConstantBufferByName(desc.Name);

		//!< constant bufferのサイズを取得して32bit定数の個数を計算する
		D3D12_SHADER_BUFFER_DESC bufferDesc = {};
		constant->GetDesc(&bufferDesc);
		declaration.num32bit = static_cast<UINT>(bufferDesc.Size / ShaderParameter::RootConstants::k32bitConstantSize);
	}

	return declaration;
}

bool ShaderBindingSlot::Declaration::IsRootConstants(ShaderInputType type, const std::string_view& name) {
	//!< 大文字から始まるConstantBufferは32bitConstantsに変更
	return type == ShaderInputType::ConstantBuffer && std::isupper(name[0]);
}

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBindingSlot structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderBindingSlot::Reconcile(const Declaration& _declaration) {
	STREAM_ASSERT(declaration == _declaration, "declaration mismatch. name: {}", declaration.name);
	visibility = ShaderVisibility::All; //!< 複数shaderで使用されるのでallに変更.
}

void ShaderBindingSlot::AppendGraphicsRootParameter(RootSignature::GraphicsDesc& desc, UINT& index, const StaticSamplerSet& samplers) {
	//!< bindingに応じて, descにルートパラメータを追加する.
	switch (declaration.GetBindingType()) {
		case BindingType::RootConstants:
			desc.Set32bitConstants(index, visibility, declaration.num32bit.value(), declaration.registerNumber, declaration.registerSpace);
			break;

		case BindingType::VirtualAddressCBV:
			desc.SetVirtualAddressCBV(index, visibility, declaration.registerNumber, declaration.registerSpace);
			break;

		case BindingType::VirtualAddressSRV:
			desc.SetVirtualAddressSRV(index, visibility, declaration.registerNumber, declaration.registerSpace);
			break;

		case BindingType::VirtualAddressUAV:
			desc.SetVirtualAddressUAV(index, visibility, declaration.registerNumber, declaration.registerSpace);
			break;

		case BindingType::HandleSRV:
			desc.SetDescriptorHandleSRV(index, visibility, declaration.registerNumber, declaration.registerSpace);
			break;

		case BindingType::HandleUAV:
			desc.SetDescriptorHandleUAV(index, visibility, declaration.registerNumber, declaration.registerSpace);
			break;

		case BindingType::Sampler: // note: 現在はStaticSamplerのみサポート.
			if (samplers.Contains(declaration.name)) {
				desc.AppendSamplerDesc(samplers.GetSampler(declaration.name, visibility, declaration.registerNumber, declaration.registerSpace)); //!< StaticSamplerの設定.

			} else {
				desc.AppendSamplerLinear(SampleMode::Wrap, visibility, declaration.registerNumber, declaration.registerSpace); //!< デフォルトのSamplerの設定.
			}
			return;
	}

	//!< root parameterのindexを更新する.
	rootParameterIndex = index;
	index++;
}

void ShaderBindingSlot::AppendComputeRootParameter(RootSignature::ComputeDesc& desc, UINT& index, const StaticSamplerSet& samplers) {
	STREAM_ASSERT(visibility == ShaderVisibility::All, "compute shader only supports All visibility.");

	//!< bindingに応じて, descにルートパラメータを追加する.
	switch (declaration.GetBindingType()) {
		case BindingType::RootConstants:
			desc.Set32bitConstants(index, visibility, declaration.num32bit.value(), declaration.registerNumber, declaration.registerSpace);
			break;

		case BindingType::VirtualAddressCBV:
			desc.SetVirtualAddressCBV(index, declaration.registerNumber, declaration.registerSpace);
			break;

		case BindingType::VirtualAddressSRV:
			desc.SetVirtualAddressSRV(index, declaration.registerNumber, declaration.registerSpace);
			break;

		case BindingType::VirtualAddressUAV:
			desc.SetVirtualAddressUAV(index, declaration.registerNumber, declaration.registerSpace);
			break;

		case BindingType::HandleSRV:
			desc.SetDescriptorHandleSRV(index, declaration.registerNumber, declaration.registerSpace);
			break;

		case BindingType::HandleUAV:
			desc.SetDescriptorHandleUAV(index, declaration.registerNumber, declaration.registerSpace);
			break;

		case BindingType::Sampler: // note: 現在はStaticSamplerのみサポート.
			if (samplers.Contains(declaration.name)) {
				desc.AppendSamplerDesc(samplers.GetSampler(declaration.name, visibility, declaration.registerNumber, declaration.registerSpace)); //!< StaticSamplerの設定.

			} else {
				desc.AppendSamplerLinear(SampleMode::Wrap, declaration.registerNumber, declaration.registerSpace); //!< デフォルトのSamplerの設定.
			}
			return;
	}

	//!< root parameterのindexを更新する.
	rootParameterIndex = index;
	index++;
}

void ShaderBindingSlot::BindGraphicsRootParameter(const GraphicsCommandContext& context, const ShaderParameter& parameter) const {
	if (declaration.GetBindingType() == BindingType::Sampler) {
		//!< SamplerはStaticSamplerとしてルートシグネチャに設定されるので, ここでは何もしない.
		return;
	}

	//!< root parameter indexの取得
	STREAM_ASSERT(rootParameterIndex.has_value(), "root parameter index is not set. name: {}", declaration.name);
	UINT index = rootParameterIndex.value();

	STREAM_ASSERT(parameter.Contains(declaration.name), "parameter not found. name: {}", declaration.name);

	auto commandList = context.GetCommandList();

	//!< bindingに応じて, parameterをcommandListにバインドする.
	switch (declaration.GetBindingType()) {
		case BindingType::RootConstants:
			{
				const ShaderParameter::RootConstants& constants = parameter.Get32bitConstants(declaration.name);
				commandList->SetGraphicsRoot32BitConstants(index, constants.num32bit, constants.data, 0);
			}
			break;

		case BindingType::VirtualAddressCBV:
			commandList->SetGraphicsRootConstantBufferView(index, parameter.GetAddress(declaration.name));
			break;

		case BindingType::VirtualAddressSRV:
			commandList->SetGraphicsRootShaderResourceView(index, parameter.GetAddress(declaration.name));
			break;

		case BindingType::VirtualAddressUAV:
			commandList->SetGraphicsRootUnorderedAccessView(index, parameter.GetAddress(declaration.name));
			break;

		case BindingType::HandleSRV:
		case BindingType::HandleUAV:
			commandList->SetGraphicsRootDescriptorTable(index, parameter.GetHandle(declaration.name));
			break;
	}
}

void ShaderBindingSlot::BindComputeRootParameter(const GraphicsCommandContext& context, const ShaderParameter& parameter) const {
	if (declaration.GetBindingType() == BindingType::Sampler) {
		//!< SamplerはStaticSamplerとしてルートシグネチャに設定されるので, ここでは何もしない.
		return;
	}

	//!< root parameter indexの取得
	STREAM_ASSERT(rootParameterIndex.has_value(), "root parameter index is not set. name: {}", declaration.name);
	UINT index = rootParameterIndex.value();

	STREAM_ASSERT(parameter.Contains(declaration.name), "parameter not found. name: {}", declaration.name);

	auto commandList = context.GetCommandList();

	//!< bindingに応じて, parameterをcommandListにバインドする.
	switch (declaration.GetBindingType()) {
		case BindingType::RootConstants:
			{
				const ShaderParameter::RootConstants& constants = parameter.Get32bitConstants(declaration.name);
				commandList->SetComputeRoot32BitConstants(index, constants.num32bit, constants.data, 0);
			}
			break;

		case BindingType::VirtualAddressCBV:
			commandList->SetComputeRootConstantBufferView(index, parameter.GetAddress(declaration.name));
			break;

		case BindingType::VirtualAddressSRV:
			commandList->SetComputeRootShaderResourceView(index, parameter.GetAddress(declaration.name));
			break;

		case BindingType::VirtualAddressUAV:
			commandList->SetComputeRootUnorderedAccessView(index, parameter.GetAddress(declaration.name));
			break;

		case BindingType::HandleSRV:
		case BindingType::HandleUAV:
			commandList->SetComputeRootDescriptorTable(index, parameter.GetHandle(declaration.name));
			break;
	}
}
