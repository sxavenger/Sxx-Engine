#include "ShaderBindingLayout.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBindingLayout class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderBindingLayout::Reflect(ShaderVisibility visibility, const ShaderReflection& reflection) {

	//!< shader全体の情報を取得
	D3D12_SHADER_DESC main = reflection.GetShaderDesc();

	for (UINT i = 0; i < main.BoundResources; ++i) {
		//!< bind情報の取得
		D3D12_SHADER_INPUT_BIND_DESC desc = reflection.GetResourceBindingDesc(i);

		Insert(visibility, desc, reflection); //!< bind情報の追加
	}
}

void ShaderBindingLayout::Reset() {
	slots_.clear();
}

bool ShaderBindingLayout::Contains(const std::string& name) const {
	return slots_.contains(name);
}

ShaderBindingSlot& ShaderBindingLayout::GetSlot(const std::string& name) {
	StreamLogger::Assert(slots_.contains(name), std::format("shader bind slot not found. name: {}", name));
	return slots_.at(name);
}

const ShaderBindingSlot& ShaderBindingLayout::GetSlot(const std::string& name) const {
	StreamLogger::Assert(slots_.contains(name), std::format("shader bind slot not found. name: {}", name));
	return slots_.at(name);
}

RootSignature ShaderBindingLayout::CreateGraphicsRootSignature(const Device& device, D3D12_ROOT_SIGNATURE_FLAGS flags) {

	//!< root signature descの作成
	RootSignature::GraphicsDesc desc = {};
	UINT index = 0;

	for (auto& slot : slots_ | std::views::values) {
		slot.AppendGraphicsRootParameter(desc, index);
	}

	//!< root signatureの作成
	return RootSignature::Create(device, desc, flags);
}

RootSignature ShaderBindingLayout::CreateGraphicsRootSignature(const Device& device, const StaticSamplerSet& samplers, D3D12_ROOT_SIGNATURE_FLAGS flags) {
	//!< root signature descの作成
	RootSignature::GraphicsDesc desc = {};
	UINT index = 0;

	for (auto& slot : slots_ | std::views::values) {
		slot.AppendGraphicsRootParameter(desc, index, samplers);
	}

	//!< root signatureの作成
	return RootSignature::Create(device, desc, flags);
	
}

RootSignature ShaderBindingLayout::CreateComputeRootSignature(const Device& device, D3D12_ROOT_SIGNATURE_FLAGS flags) {

	//!< root signature descの作成
	RootSignature::ComputeDesc desc = {};
	UINT index = 0;

	for (auto& slot : slots_ | std::views::values) {
		slot.AppendComputeRootParameter(desc, index);
	}

	//!< root signatureの作成
	return RootSignature::Create(device, desc, flags);
}

RootSignature ShaderBindingLayout::CreateComputeRootSignature(const Device& device, const StaticSamplerSet& samplers, D3D12_ROOT_SIGNATURE_FLAGS flags) {
	//!< root signature descの作成
	RootSignature::ComputeDesc desc = {};
	UINT index = 0;

	for (auto& slot : slots_ | std::views::values) {
		slot.AppendComputeRootParameter(desc, index, samplers);
	}

	//!< root signatureの作成
	return RootSignature::Create(device, desc, flags);
}

void ShaderBindingLayout::BindGraphicsRootParameter(const GraphicsCommandContext& context, const ShaderParameter& parameter) {
	for (const auto& slot : slots_ | std::views::values) {
		slot.BindGraphicsRootParameter(context, parameter);
	}
}

void ShaderBindingLayout::BindComputeRootParameter(const GraphicsCommandContext& context, const ShaderParameter& parameter) {
	for (const auto& slot : slots_ | std::views::values) {
		slot.BindComputeRootParameter(context, parameter);
	}
}

void ShaderBindingLayout::Insert(ShaderVisibility visibility, const D3D12_SHADER_INPUT_BIND_DESC& desc, const ShaderReflection& reflection) {

	//!< shader定義での宣言情報の取得.
	ShaderBindingSlot::Declaration declaration = ShaderBindingSlot::Declaration::Create(desc, reflection);

	if (Contains(declaration.name)) {
		//!< 既に同じ名前のバインドが存在する場合は、visibilityを更新.

		ShaderBindingSlot& slot = GetSlot(declaration.name); //!< 既存のslotを取得
		slot.Reconcile(declaration); //!< slotの宣言情報を更新
		return;
	}

	//!< 新しいslotを作成
	ShaderBindingSlot slot = {};
	slot.rootParameterIndex = std::nullopt; //!< root signature作成時に設定される.
	slot.visibility         = visibility;
	slot.declaration        = declaration;

	//!< 新しいslotを追加
	slots_.emplace(declaration.name, slot);
}
