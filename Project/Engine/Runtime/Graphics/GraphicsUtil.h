#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Utility/ComPtr.h>

//* lib
#include <Lib/Pointer/ReferencePointer.h>

//* DirectX12
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <d3d12.h>
#include <d3d12shader.h>

//* c++
#include <cstdint>

//-----------------------------------------------------------------------------------------
// pragma comment
//-----------------------------------------------------------------------------------------
//!< DirectX12
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorCategory enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class DescriptorCategory : uint8_t {
	RTV,         //!< RenderTargetView
	DSV,         //!< DepthStencilView
	SRV_CBV_UAV, //!< ShaderResourceView, ConstantBufferView, UnorderedAccessView

	SRV = SRV_CBV_UAV, //!< ShaderResourceView
	UAV = SRV_CBV_UAV, //!< UnorderedAccessView
	CBV = SRV_CBV_UAV, //!< ConstantBufferView
};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsCommandType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class GraphicsCommandType : uint8_t {
	Direct,
	Compute,
	Copy,
};

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderVisibility enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class ShaderVisibility : uint32_t {
	All           = D3D12_SHADER_VISIBILITY_ALL,
	Vertex        = D3D12_SHADER_VISIBILITY_VERTEX,
	Hull          = D3D12_SHADER_VISIBILITY_HULL,
	Domain        = D3D12_SHADER_VISIBILITY_DOMAIN,
	Geometry      = D3D12_SHADER_VISIBILITY_GEOMETRY,
	Pixel         = D3D12_SHADER_VISIBILITY_PIXEL,
	Amplification = D3D12_SHADER_VISIBILITY_AMPLIFICATION,
	Mesh          = D3D12_SHADER_VISIBILITY_MESH
};

////////////////////////////////////////////////////////////////////////////////////////////
// SampleFilter enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class SampleFilter : uint32_t {
	Linear      = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
	Point       = D3D12_FILTER_MIN_MAG_MIP_POINT,
	Anisotropic = D3D12_FILTER_ANISOTROPIC
};

////////////////////////////////////////////////////////////////////////////////////////////
// SampleMode enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class SampleMode : uint32_t {
	Wrap       = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	Mirror     = D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
	Clamp      = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	Border     = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
	MirrorOnce = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE
};

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveTopology enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class PrimitiveTopology : uint32_t {
	Undefined     = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED,
	Point         = D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
	Line          = D3D_PRIMITIVE_TOPOLOGY_LINELIST,
	LineStrip     = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
	Triangle      = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	TriangleStrip = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
};

////////////////////////////////////////////////////////////////////////////////////////////
// CompileProfile enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class CompileProfile : uint8_t {
	Vertex,
	Geometry,
	Hull,
	Domain,
	Mesh,
	Amplification,
	Pixel,
	Compute,
	Library
};

////////////////////////////////////////////////////////////////////////////////////////////
// ColorEncoding enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class ColorEncoding : bool {
	Intensity, //!< Linear Color Space (Normal Map etc...)
	Lightness, //!< sRGB Color Space (Main Color etc...)
};

////////////////////////////////////////////////////////////////////////////////////////////
// Utility methods
////////////////////////////////////////////////////////////////////////////////////////////

constexpr DXGI_FORMAT ConvertToSRGBFormat(DXGI_FORMAT format) {
	switch (format) {
		case DXGI_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case DXGI_FORMAT_BC1_UNORM:      return DXGI_FORMAT_BC1_UNORM_SRGB;
		case DXGI_FORMAT_BC2_UNORM:      return DXGI_FORMAT_BC2_UNORM_SRGB;
		case DXGI_FORMAT_BC3_UNORM:      return DXGI_FORMAT_BC3_UNORM_SRGB;
		case DXGI_FORMAT_B8G8R8A8_UNORM: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		case DXGI_FORMAT_B8G8R8X8_UNORM: return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
		case DXGI_FORMAT_BC7_UNORM:      return DXGI_FORMAT_BC7_UNORM_SRGB;
		default:                         return format;
	}
}

constexpr DXGI_FORMAT ConvertToLinearFormat(DXGI_FORMAT format) {
	switch (format) {
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case DXGI_FORMAT_BC1_UNORM_SRGB:      return DXGI_FORMAT_BC1_UNORM;
		case DXGI_FORMAT_BC2_UNORM_SRGB:      return DXGI_FORMAT_BC2_UNORM;
		case DXGI_FORMAT_BC3_UNORM_SRGB:      return DXGI_FORMAT_BC3_UNORM;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8X8_UNORM;
		case DXGI_FORMAT_BC7_UNORM_SRGB:      return DXGI_FORMAT_BC7_UNORM;
		default:                              return format;
	}
}

constexpr DXGI_FORMAT ConvertToDepthViewFormat(DXGI_FORMAT format) {
	switch (format) {
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		case DXGI_FORMAT_D32_FLOAT:            return DXGI_FORMAT_R32_FLOAT;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:    return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case DXGI_FORMAT_D16_UNORM:            return DXGI_FORMAT_R16_UNORM;
		default:                               return format;
	}
}

constexpr D3D12_COMMAND_LIST_TYPE GetCommandListType(GraphicsCommandType type) {
	switch (type) {
		case GraphicsCommandType::Direct:  return D3D12_COMMAND_LIST_TYPE_DIRECT;
		case GraphicsCommandType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		case GraphicsCommandType::Copy:    return D3D12_COMMAND_LIST_TYPE_COPY;
		default:                           return D3D12_COMMAND_LIST_TYPE_BUNDLE;
	}
}

constexpr std::wstring_view GetCompileShaderStage(CompileProfile profile) {
	switch (profile) {
		case CompileProfile::Vertex:        return L"vs";
		case CompileProfile::Geometry:      return L"gs";
		case CompileProfile::Hull:          return L"hs";
		case CompileProfile::Domain:        return L"ds";
		case CompileProfile::Mesh:          return L"ms";
		case CompileProfile::Amplification: return L"as";
		case CompileProfile::Pixel:         return L"ps";
		case CompileProfile::Compute:       return L"cs";
		case CompileProfile::Library:       return L"lib";
		default:                            return L"unknown";
	}
}

constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(PrimitiveTopology topology) {
	switch (topology) {
		case PrimitiveTopology::Point:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		case PrimitiveTopology::Line:
		case PrimitiveTopology::LineStrip:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

		case PrimitiveTopology::Triangle:
		case PrimitiveTopology::TriangleStrip:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		default:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}
}

constexpr ColorEncoding GetColorEncoding(DXGI_FORMAT format) {
	switch (format) {
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return ColorEncoding::Lightness;

		default:
			return ColorEncoding::Intensity;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// constraint
////////////////////////////////////////////////////////////////////////////////////////////

constexpr const D3D_SHADER_MODEL kRequireShaderModel = D3D_SHADER_MODEL_6_5; //!< エンジン機能要件のShaderModel
static_assert(kRequireShaderModel >= D3D_SHADER_MODEL_6_5, "engine requires Shader Model 6.5 or higher");

constexpr const size_t kFrameCount = 2; //!< フレームリソースの数

SXAVENGER_ENGINE_NAMESPACE_END
