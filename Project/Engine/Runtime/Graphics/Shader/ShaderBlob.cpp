#include "ShaderBlob.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "ShaderCompiler.h"

//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

D3D12_SHADER_BYTECODE ShaderBlob::GetBytecode() const {
	STREAM_ASSERT(blob_ != nullptr, "blob is null.");

	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength  = blob_->GetBufferSize();
	bytecode.pShaderBytecode = blob_->GetBufferPointer();

	return bytecode;
}

DxcBuffer ShaderBlob::GetBuffer(CodePage codePage) const {
	STREAM_ASSERT(blob_ != nullptr, "blob is null.");

	DxcBuffer buffer = {};
	buffer.Size     = blob_->GetBufferSize();
	buffer.Ptr      = blob_->GetBufferPointer();
	buffer.Encoding = static_cast<UINT>(codePage);

	return buffer;
}

ShaderReflection ShaderBlob::Reflect() const {
	STREAM_ASSERT(blob_ != nullptr, "blob is null.");
	return compiler_->Reflect(*this);
}

