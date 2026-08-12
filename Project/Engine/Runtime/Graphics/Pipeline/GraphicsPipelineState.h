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
#include "BlendState.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Math/Vector2.h>

//* c++
#include <vector>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsPipelineState class
////////////////////////////////////////////////////////////////////////////////////////////
class GraphicsPipelineState {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GraphicsType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class GraphicsType : bool {
		Vertex, //!< Vertex Shader Pipeline
		Mesh,   //!< Mesh Shader Pipeline
	};

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

		const ShaderBlob& GetShaderBlob(CompileProfile profile) const;

		D3D12_SHADER_BYTECODE GetShaderBytecode(CompileProfile profile) const;

		//* element option *//

		void ResetElement();

		void AppendElement(const LPCSTR& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT inputSlot = 0);

		void SetElement(size_t index, const LPCSTR& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT inputSlot = 0);

		D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc() const;

		//* rasterizer option *//

		void SetRasterizer(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode, bool enableDepthClip = true);

		//* depth stencil option *//

		void SetDepthStencil(bool enable, D3D12_DEPTH_WRITE_MASK writeMask = D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL);

		//* blend option *//

		void ResetBlendState();

		void SetBlendState(uint8_t index, const D3D12_RENDER_TARGET_BLEND_DESC& desc);

		void SetBlendMode(uint8_t index, BlendModeColor color, BlendModeTransparent transparent);

		void SetIndependentBlendEnable(bool enable);

		D3D12_BLEND_DESC GetBlendDesc() const;

		//* primitive option *//

		void SetPrimitive(PrimitiveTopology primitive);

		//* render target format option *//

		void ResetRenderTargetFormat();

		void AppendRenderTargetFormat(DXGI_FORMAT format);

		void SetRenderTargetFormat(uint8_t index, DXGI_FORMAT format);

		//* depth stencil format option *//

		void ResetDepthStencilFormat();

		void SetDepthStencilFormat(DXGI_FORMAT format);

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* shader blob parameter *//

		std::array<ShaderBlob, EnumUtil<CompileProfile>::Cast(CompileProfile::Pixel) + 1> blobs;
		GraphicsType type = GraphicsType::Vertex;

		//* desc parameter *//

		std::vector<D3D12_INPUT_ELEMENT_DESC> elements;
		//!< [D3D12_INPUT_LAYOUT_DESC](https://learn.microsoft.com/windows/win32/api/d3d12/ns-d3d12-d3d12_input_layout_desc)

		D3D12_RASTERIZER_DESC rasterizer = {};
		//!< [D3D12_RASTERIZER_DESC](https://learn.microsoft.com/windows/win32/api/d3d12/ns-d3d12-d3d12_rasterizer_desc)

		D3D12_DEPTH_STENCIL_DESC depthStencil = {};
		//!< [D3D12_DEPTH_STENCIL_DESC](https://learn.microsoft.com/windows/win32/api/d3d12/ns-d3d12-d3d12_depth_stencil_desc)

		std::array<D3D12_RENDER_TARGET_BLEND_DESC, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> blends = {};
		bool isIndependentBlendEnable = false;
		//!< [D3D12_BLEND_DESC](https://learn.microsoft.com/windows/win32/api/d3d12/ns-d3d12-d3d12_blend_desc)

		//* primitive parameter *//

		PrimitiveTopology primitiveTopology = PrimitiveTopology::Undefined;

		//* format parameter *//

		std::vector<DXGI_FORMAT> rtvFormats = {};
		DXGI_FORMAT dsvFormat               = DXGI_FORMAT_UNKNOWN;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* pipeline state option *//

	void SetName(const std::wstring_view& name) const;
	void SetName(const std::string_view& name) const;

	void BindPipeline(const GraphicsCommandContext& context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) const;
	void BindPipeline(const GraphicsCommandContext& context, const Vector2ui& resolution) const;

	//* operator [comparison] <std::nullptr_t> *//

	bool operator==(std::nullptr_t) const { return pipeline_ == nullptr; }
	bool operator!=(std::nullptr_t) const { return pipeline_ != nullptr; }

	//* static methods *//

	static GraphicsPipelineState Create(const Device& device, const RootSignature& rootSignature, const Desc& desc);

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12PipelineState> pipeline_ = nullptr;

	//* parameter *//

	RootSignature rootSignature_;

	GraphicsType type_          = GraphicsType::Vertex;
	PrimitiveTopology topology_ = PrimitiveTopology::Undefined;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* intermediate methods *//

	static void CreatePipelineState(GraphicsPipelineState& pipeline, const Device& device, const RootSignature& rootSignature, const Desc& desc);

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* create helper methods *//

	static ComPtr<ID3D12PipelineState> CreateVertexPipelineState(RefPtr<ID3D12Device8> device, RefPtr<ID3D12RootSignature> rootSignature, const Desc& desc);

	static ComPtr<ID3D12PipelineState> CreateMeshPipelineState(RefPtr<ID3D12Device8> device, RefPtr<ID3D12RootSignature> rootSignature, const Desc& desc);

};

SXAVENGER_ENGINE_NAMESPACE_END
