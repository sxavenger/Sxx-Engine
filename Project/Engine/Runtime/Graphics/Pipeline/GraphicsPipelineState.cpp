#include "GraphicsPipelineState.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/String/UnicodeConverter.h>

//* DirectX12
#include <DirectXTex/Common/d3dx12.h>

//* c++
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// [GraphicsPipelineState] Desc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipelineState::Desc::ResetShaderBlob() {
	blobs.fill(nullptr);
	type = GraphicsType::Vertex;
}

void GraphicsPipelineState::Desc::SetShaderBlob(const ShaderBlob& blob) {

	CompileProfile profile = blob.GetProfile();
	StreamLogger::Assert(
		profile != CompileProfile::Library && profile != CompileProfile::Compute,
		std::format("invalid shader blob profile. profile: {}", profile)
	); //!< profileの範囲チェック (GraphicsPipelineStateのDescではLibraryとComputeは無効)

	blobs[EnumUtil<CompileProfile>::Cast(profile)] = blob;

	switch (profile) { //!< profileに応じてGraphicsTypeを設定
		case CompileProfile::Vertex:
			type = GraphicsType::Vertex;
			break;

		case CompileProfile::Mesh:
			type = GraphicsType::Mesh;
			break;
	}
}

const ShaderBlob& GraphicsPipelineState::Desc::GetShaderBlob(CompileProfile profile) const {
	StreamLogger::Assert(
		profile != CompileProfile::Library && profile != CompileProfile::Compute,
		std::format("invalid shader blob profile. profile: {}", profile)
	); //!< profileの範囲チェック (GraphicsPipelineStateのDescではLibraryとComputeは無効)

	return blobs[EnumUtil<CompileProfile>::Cast(profile)];
}

D3D12_SHADER_BYTECODE GraphicsPipelineState::Desc::GetShaderBytecode(CompileProfile profile) const {

	const ShaderBlob& blob = blobs[EnumUtil<CompileProfile>::Cast(profile)];

	if (blob == nullptr) {
		switch (profile) {
			case CompileProfile::Vertex:
			case CompileProfile::Mesh:
			case CompileProfile::Pixel:
				StreamLogger::Exception(std::format("required shader blob is not set. profile: {}", profile));
				//!< これらのprofileはGraphicsPipelineStateのDescで必須のため、blobがnullptrの場合は例外を投げる

			default:
				return D3D12_SHADER_BYTECODE{};
				//!< それ以外のprofileはnullを返す
		}
	}

	return blob.GetBytecode();
}

void GraphicsPipelineState::Desc::ResetElement() {
	elements.clear();
}

void GraphicsPipelineState::Desc::AppendElement(const LPCSTR& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT inputSlot) {
	D3D12_INPUT_ELEMENT_DESC element = {};
	element.SemanticName      = semanticName;
	element.SemanticIndex     = semanticIndex;
	element.Format            = format;
	element.InputSlot         = inputSlot;
	element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	elements.emplace_back(element);
}

void GraphicsPipelineState::Desc::SetElement(size_t index, const LPCSTR& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT inputSlot) {
	D3D12_INPUT_ELEMENT_DESC element = {};
	element.SemanticName      = semanticName;
	element.SemanticIndex     = semanticIndex;
	element.Format            = format;
	element.InputSlot         = inputSlot;
	element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	if (index >= elements.size()) {
		elements.resize(index + 1, {}); //!< indexの位置までサイズを拡張
	}

	elements[index] = element;
}

D3D12_INPUT_LAYOUT_DESC GraphicsPipelineState::Desc::GetInputLayoutDesc() const {
	D3D12_INPUT_LAYOUT_DESC desc = {};
	desc.pInputElementDescs = elements.data();
	desc.NumElements        = static_cast<UINT>(elements.size());

	return desc;
}

void GraphicsPipelineState::Desc::SetRasterizer(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode, bool enableDepthClip) {
	rasterizer.CullMode        = cullMode;
	rasterizer.FillMode        = fillMode;
	rasterizer.DepthClipEnable = enableDepthClip;
}

void GraphicsPipelineState::Desc::SetDepthStencil(bool enable, D3D12_DEPTH_WRITE_MASK writeMask, D3D12_COMPARISON_FUNC comparisonFunc) {
	depthStencil.DepthEnable    = enable;
	depthStencil.DepthWriteMask = writeMask;
	depthStencil.DepthFunc      = comparisonFunc;
}

void GraphicsPipelineState::Desc::ResetBlendState() {
	blends.fill(D3D12_RENDER_TARGET_BLEND_DESC{});
	isIndependentBlendEnable = false;
}

void GraphicsPipelineState::Desc::SetBlendState(uint8_t index, const D3D12_RENDER_TARGET_BLEND_DESC& desc) {
	StreamLogger::Assert(index < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, "BlendState index must be within D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT"); //!< indexの範囲チェック
	blends[index] = desc;
}

void GraphicsPipelineState::Desc::SetBlendMode(uint8_t index, BlendModeColor color, BlendModeTransparent transparent) {
	StreamLogger::Assert(index < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, "BlendState index must be within D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT"); //!< indexの範囲チェック
	blends[index] = BlendState::CreateBlendDesc(color, transparent);
}

void GraphicsPipelineState::Desc::SetIndependentBlendEnable(bool enable) {
	isIndependentBlendEnable = enable;
}

D3D12_BLEND_DESC GraphicsPipelineState::Desc::GetBlendDesc() const {
	D3D12_BLEND_DESC desc = {};
	desc.IndependentBlendEnable = isIndependentBlendEnable;
	std::ranges::copy(blends, desc.RenderTarget); //!< 配列のコピー

	return desc;
}

void GraphicsPipelineState::Desc::SetPrimitive(PrimitiveTopology primitive) {
	primitiveTopology = primitive;
}

void GraphicsPipelineState::Desc::ResetRenderTargetFormat() {
	rtvFormats.clear();
}

void GraphicsPipelineState::Desc::AppendRenderTargetFormat(DXGI_FORMAT format) {
	rtvFormats.emplace_back(format);
	StreamLogger::Assert(rtvFormats.size() < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, "Render Target Format must be within D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT"); //!< RTVの設定限界
}

void GraphicsPipelineState::Desc::SetRenderTargetFormat(uint8_t index, DXGI_FORMAT format) {
	if (rtvFormats.size() <= index) {
		rtvFormats.resize(index + 1, DXGI_FORMAT_UNKNOWN); //!< indexの位置までサイズを拡張
	}

	rtvFormats[index] = format;
	StreamLogger::Assert(rtvFormats.size() < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, "Render Target Format must be within D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT"); //!< RTVの設定限界
}

void GraphicsPipelineState::Desc::ResetDepthStencilFormat() {
	dsvFormat = DXGI_FORMAT_UNKNOWN;
}

void GraphicsPipelineState::Desc::SetDepthStencilFormat(DXGI_FORMAT format) {
	dsvFormat = format;
}

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipelineState class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipelineState::SetName(const std::wstring_view& name) const {
	pipeline_->SetName(name.data());
}

void GraphicsPipelineState::SetName(const std::string_view& name) const {
	SetName(UnicodeConverter::ConvertW(name));
}

void GraphicsPipelineState::BindPipeline(const GraphicsCommandContext& context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) const {

	auto commandList = context.GetCommandList();

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &rect);

	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(pipeline_.Get());

	if (type_ == GraphicsType::Vertex) {
		//!< Vertex Shader Pipelineの場合はプリミティブトポロジーを設定
		commandList->IASetPrimitiveTopology(static_cast<D3D12_PRIMITIVE_TOPOLOGY>(topology_));
	}

}

void GraphicsPipelineState::BindPipeline(const GraphicsCommandContext& context, const Vector2ui& resolution) const {

	//!< viewportの設定
	D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width    = static_cast<float>(resolution.x);
	viewport.Height   = static_cast<float>(resolution.y);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//!< scissor rectの設定
	D3D12_RECT rect = {};
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = static_cast<LONG>(resolution.x);
	rect.bottom = static_cast<LONG>(resolution.y);

	BindPipeline(context, viewport, rect);
}

GraphicsPipelineState GraphicsPipelineState::Create(const Device& device, const RootSignature& rootSignature, const Desc& desc) {

	GraphicsPipelineState pipeline;

	//!< pipelineStateの作成
	switch (desc.type) {
		case GraphicsType::Vertex:
			pipeline.pipeline_ = GraphicsPipelineState::CreateVertexPipelineState(device.GetDevice(), rootSignature.Get(), desc);
			break;

		case GraphicsType::Mesh:
			pipeline.pipeline_ = GraphicsPipelineState::CreateMeshPipelineState(device.GetDevice(), rootSignature.Get(), desc);
			break;
	}

	//!< 設定の保存
	pipeline.rootSignature_ = rootSignature;
	pipeline.type_          = desc.type;
	pipeline.topology_      = desc.primitiveTopology;

	return pipeline;
}

ComPtr<ID3D12PipelineState> GraphicsPipelineState::CreateVertexPipelineState(RefPtr<ID3D12Device8> device, RefPtr<ID3D12RootSignature> rootSignature, const Desc& desc) {

	//!< descの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC state = {};
	state.InputLayout           = desc.GetInputLayoutDesc();
	state.RasterizerState       = desc.rasterizer;
	state.DepthStencilState     = desc.depthStencil;
	state.PrimitiveTopologyType = GetPrimitiveTopologyType(desc.primitiveTopology);
	state.BlendState            = desc.GetBlendDesc();
	state.SampleDesc.Count      = 1;
	state.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;

	//!< render target formatの設定
	state.NumRenderTargets = static_cast<UINT>(desc.rtvFormats.size());
	std::ranges::copy(desc.rtvFormats, state.RTVFormats); //!< 配列のコピー

	//!< depth stencil formatの設定
	state.DSVFormat = desc.dsvFormat;

	//!< shaderの設定
	state.VS = desc.GetShaderBytecode(CompileProfile::Vertex);
	state.GS = desc.GetShaderBytecode(CompileProfile::Geometry);
	state.HS = desc.GetShaderBytecode(CompileProfile::Hull);
	state.DS = desc.GetShaderBytecode(CompileProfile::Domain);
	state.PS = desc.GetShaderBytecode(CompileProfile::Pixel);

	//!< root signatureの設定
	state.pRootSignature = rootSignature.Get();

	//!< pipeline stateの作成
	ComPtr<ID3D12PipelineState> pipeline;
	auto hr = device->CreateGraphicsPipelineState(
		&state,
		IID_PPV_ARGS(&pipeline)
	);
	ComPtrUtil::Assert(hr, L"vertex pipeline state creation failed");

	return pipeline;
}

ComPtr<ID3D12PipelineState> GraphicsPipelineState::CreateMeshPipelineState(RefPtr<ID3D12Device8> device, RefPtr<ID3D12RootSignature> rootSignature, const Desc& desc) {

	//!< descの設定
	D3DX12_MESH_SHADER_PIPELINE_STATE_DESC state = {};
	state.RasterizerState       = desc.rasterizer;
	state.DepthStencilState     = desc.depthStencil;
	state.PrimitiveTopologyType = GetPrimitiveTopologyType(desc.primitiveTopology);
	state.BlendState            = desc.GetBlendDesc();
	state.SampleDesc.Count      = 1;
	state.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;

	//!< render target formatの設定
	state.NumRenderTargets = static_cast<UINT>(desc.rtvFormats.size());
	std::ranges::copy(desc.rtvFormats, state.RTVFormats); //!< 配列のコピー

	//!< depth stencil formatの設定
	state.DSVFormat = desc.dsvFormat;

	//!< shaderの設定
	state.AS = desc.GetShaderBytecode(CompileProfile::Amplification);
	state.MS = desc.GetShaderBytecode(CompileProfile::Mesh);
	state.PS = desc.GetShaderBytecode(CompileProfile::Pixel);

	//!< root signatureの設定
	state.pRootSignature = rootSignature.Get();

	//!< pipeline stateの作成
	CD3DX12_PIPELINE_MESH_STATE_STREAM stream = CD3DX12_PIPELINE_MESH_STATE_STREAM(state);

	D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = {};
	streamDesc.pPipelineStateSubobjectStream = &stream;
	streamDesc.SizeInBytes                   = sizeof(CD3DX12_PIPELINE_MESH_STATE_STREAM);

	ComPtr<ID3D12PipelineState> pipeline;
	auto hr = device->CreatePipelineState(
		&streamDesc,
		IID_PPV_ARGS(&pipeline)
	);
	ComPtrUtil::Assert(hr, L"mesh pipeline state creation failed.");

	return pipeline;
}
