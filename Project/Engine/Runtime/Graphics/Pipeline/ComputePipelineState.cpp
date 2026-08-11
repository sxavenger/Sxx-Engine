#include "ComputePipelineState.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/String/UnicodeConverter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [ComputePipelineState] Desc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ComputePipelineState::Desc::ResetShaderBlob() {
	blob = nullptr;
}

void ComputePipelineState::Desc::SetShaderBlob(const ShaderBlob& _blob) {

	CompileProfile profile = _blob.GetProfile();
	StreamLogger::Assert(
		profile == CompileProfile::Compute,
		std::format("invalid shader blob profile. profile: {}", profile)
	); //!< profileの範囲チェック (ComputePipelineStateのDescではComputeのみ有効)

	blob = _blob;
}

D3D12_SHADER_BYTECODE ComputePipelineState::Desc::GetShaderBytecode() const {
	StreamLogger::Assert(blob != nullptr, "shader blob is not set.");  //!< blobが設定されていない
	return blob.GetBytecode();
}

////////////////////////////////////////////////////////////////////////////////////////////
// ComputePipelineState class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ComputePipelineState::SetName(const std::wstring_view& name) const {
	pipeline_->SetName(name.data());
}

void ComputePipelineState::SetName(const std::string_view& name) const {
	SetName(UnicodeConverter::ConvertW(name));
}

void ComputePipelineState::BindPipeline(const GraphicsCommandContext& context) const {

	auto commandList = context.GetCommandList();

	commandList->SetPipelineState(pipeline_.Get());
	commandList->SetComputeRootSignature(rootSignature_.Get());

}

ComputePipelineState ComputePipelineState::Create(const Device& device, const RootSignature& rootSignature, const Desc& desc) {

	ComputePipelineState pipeline;

	//!< pipelineの作成
	pipeline.pipeline_ = ComputePipelineState::CreateComputePipelineState(device.GetDevice(), rootSignature.Get(), desc);

	//!< 設定の保存
	pipeline.rootSignature_ = rootSignature;

	return pipeline;
}

ComPtr<ID3D12PipelineState> ComputePipelineState::CreateComputePipelineState(RefPtr<ID3D12Device8> device, RefPtr<ID3D12RootSignature> rootSignature, const Desc& desc) {

	//!< descの設定
	D3D12_COMPUTE_PIPELINE_STATE_DESC state = {};
	state.CS = desc.GetShaderBytecode();

	//!< root signatureの設定
	state.pRootSignature = rootSignature.Get();

	ComPtr<ID3D12PipelineState> pipeline;
	auto hr = device->CreateComputePipelineState(
		&state,
		IID_PPV_ARGS(&pipeline)
	);
	ComPtrUtil::Assert(hr, L"compute pipeline state creation failed.");

	return pipeline;
}
