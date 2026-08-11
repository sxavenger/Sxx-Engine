#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Core/GraphicsCommandContext.h"
#include "../Shader/ShaderReflection.h"
#include "ShaderParameter.h"
#include "StaticSamplerSet.h"
#include "RootSignature.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBindingSlot structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ShaderBindingSlot final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ShaderInputType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class ShaderInputType : uint8_t {
		ConstantBuffer                       = D3D_SIT_CBUFFER,
		Texture                              = D3D_SIT_TEXTURE,
		Sampler                              = D3D_SIT_SAMPLER,
		StructuredBuffer                     = D3D_SIT_STRUCTURED,
		RWTexture                            = D3D_SIT_UAV_RWTYPED,
		RWStructuredBuffer                   = D3D_SIT_UAV_RWSTRUCTURED,
		RWStructuredBufferWithCounter        = D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER,
		ByteAddressBuffer                    = D3D_SIT_BYTEADDRESS,
		RWByteAddressBuffer                  = D3D_SIT_UAV_RWBYTEADDRESS,
		AppendStructuredBuffer               = D3D_SIT_UAV_APPEND_STRUCTURED,
		ConsumeStructuredBuffer              = D3D_SIT_UAV_CONSUME_STRUCTURED,
		AccelerationStructure                = D3D_SIT_RTACCELERATIONSTRUCTURE
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// BindingType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class BindingType : uint8_t {
		RootConstants,
		VirtualAddressCBV,
		VirtualAddressSRV,
		VirtualAddressUAV,
		HandleSRV,
		HandleUAV,
		Sampler
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Declaration structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief シェーダーのルートパラメータにバインドするデータの宣言情報を保持する構造体
	struct Declaration {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		BindingType GetBindingType() const;

		//* operator [comparison] <Declaration> *//

		bool operator==(const Declaration&) const = default;
		bool operator!=(const Declaration&) const = default;

		//* static methods *//

		static Declaration Create(const D3D12_SHADER_INPUT_BIND_DESC& desc, const ShaderReflection& reflection);

		static bool IsRootConstants(ShaderInputType type, const std::string_view& name);

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::string name;

		ShaderInputType type;

		UINT registerNumber;
		UINT registerSpace;

		std::optional<UINT> num32bit; //!< 32bit定数の個数(32bit定数の場合のみ有効)

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Reconcile(const Declaration& _declaration);

	//* root signature option *//

	void AppendGraphicsRootParameter(RootSignature::GraphicsDesc& desc, UINT& index, const StaticSamplerSet& samplers = {});

	void AppendComputeRootParameter(RootSignature::ComputeDesc& desc, UINT& index, const StaticSamplerSet& samplers = {});

	//* bind parameter option *//

	void BindGraphicsRootParameter(const GraphicsCommandContext& context, const ShaderParameter& parameter) const;

	void BindComputeRootParameter(const GraphicsCommandContext& context, const ShaderParameter& parameter) const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::optional<UINT> rootParameterIndex; //!< root signatureのルートパラメータインデックス
	ShaderVisibility visibility;
	Declaration declaration;

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
