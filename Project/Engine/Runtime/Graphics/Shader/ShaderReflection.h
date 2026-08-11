#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <string_view>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderReflection class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderReflection final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ShaderReflection() noexcept = default;
	ShaderReflection(ComPtr<ID3D12ShaderReflection> reflection) noexcept : reflection_(reflection) {}

	//* shader reflection option *//

	D3D12_SHADER_DESC GetShaderDesc() const;

	D3D12_SHADER_INPUT_BIND_DESC GetResourceBindingDesc(UINT index) const;

	ID3D12ShaderReflectionConstantBuffer* GetConstantBufferByName(const std::string_view& name) const;

	//* operator [copy / move] <ShaderBlob> *//

	ShaderReflection(const ShaderReflection&)            = default;
	ShaderReflection& operator=(const ShaderReflection&) = default;

	ShaderReflection(ShaderReflection&&)            = default;
	ShaderReflection& operator=(ShaderReflection&&) = default;

	//* operator [copy] <std::nullptr_t> *//

	ShaderReflection(std::nullptr_t) : reflection_(nullptr) {}
	ShaderReflection& operator=(std::nullptr_t) { reflection_ = nullptr; return *this; }

	//* operator [comparison] <std::nullptr_t> *//

	bool operator==(std::nullptr_t) const { return reflection_ == nullptr; }
	bool operator!=(std::nullptr_t) const { return reflection_ != nullptr; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12ShaderReflection> reflection_;

};

SXAVENGER_ENGINE_NAMESPACE_END
