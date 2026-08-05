#include "ShaderBlob.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

D3D12_SHADER_BYTECODE ShaderBlob::GetBytecode() const {
	StreamLogger::Assert(blob_ != nullptr, "blob is null.");

	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength  = blob_->GetBufferSize();
	bytecode.pShaderBytecode = blob_->GetBufferPointer();

	return bytecode;
}

DxcBuffer ShaderBlob::GetBuffer() const {
	StreamLogger::Assert(blob_ != nullptr, "blob is null.");

	DxcBuffer buffer = {};
	buffer.Size     = blob_->GetBufferSize();
	buffer.Ptr      = blob_->GetBufferPointer();
	buffer.Encoding = DXC_CP_UTF8;

	return buffer;
}

