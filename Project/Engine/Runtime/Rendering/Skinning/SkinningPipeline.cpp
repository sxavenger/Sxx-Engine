#include "SkinningPipeline.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Foo class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkinningPipeline::Init() {

	//!< pipelineの作成
	Graphics::ComputePipelineState::Desc desc;
	desc.SetShaderBlob(Graphics::Core::CompileShader(L"Engine/Packages/shaders/Skinning/skinning.cs.hlsl", Graphics::CompileProfile::Compute));

	pipeline_ = Graphics::ReflectedComputePipelineState::Create(Graphics::Core::GetDevice(), desc);
}

void SkinningPipeline::BindPipeline(const Graphics::GraphicsCommandContext& command) {
	pipeline_.BindPipeline(command);
}

void SkinningPipeline::BindShaderParameter(const Graphics::GraphicsCommandContext& command, const Graphics::ShaderParameter& parameter) {
	pipeline_.BindShaderParameter(command, parameter);
}

void SkinningPipeline::Dispatch(const Graphics::GraphicsCommandContext& command, uint32_t vertexCount) {
	Graphics::ThreadGroupSize threadGroupSize = pipeline_.GetThreadGroupSize();
	command.GetCommandList()->Dispatch(Graphics::ThreadGroupSize::RoundUp(vertexCount, threadGroupSize.x), 1, 1);
}
