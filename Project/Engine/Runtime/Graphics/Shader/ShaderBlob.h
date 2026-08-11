#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderBlob final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// CodePage enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class CodePage : UINT {
		ACP   = DXC_CP_ACP,
		UTF8  = DXC_CP_UTF8
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ShaderBlob() noexcept = default;
	ShaderBlob(ComPtr<IDxcBlob> blob, CompileProfile profile) noexcept : blob_(blob), profile_(profile) {}

	//* shader blob option *//

	RefPtr<IDxcBlob> GetBlob() const { return blob_.Get(); }

	D3D12_SHADER_BYTECODE GetBytecode() const;

	DxcBuffer GetBuffer(CodePage codePage = CodePage::UTF8) const;

	//* compile profile option *//

	CompileProfile GetProfile() const { return profile_; }

	//* operator [copy / move] <ShaderBlob> *//

	ShaderBlob(const ShaderBlob&)            = default;
	ShaderBlob& operator=(const ShaderBlob&) = default;

	ShaderBlob(ShaderBlob&&)            = default;
	ShaderBlob& operator=(ShaderBlob&&) = default;

	//* operator [copy] <std::nullptr_t> *//

	ShaderBlob(std::nullptr_t) : blob_(nullptr) {}
	ShaderBlob& operator=(std::nullptr_t) { blob_ = nullptr; return *this; }

	//* operator [comparison] <std::nullptr_t> *//

	bool operator==(std::nullptr_t) const { return blob_ == nullptr; }
	bool operator!=(std::nullptr_t) const { return blob_ != nullptr; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<IDxcBlob> blob_;

	//* profile *//

	CompileProfile profile_;

};

SXAVENGER_ENGINE_NAMESPACE_END
