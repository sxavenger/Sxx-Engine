#include "TriangleIndexDimensionBuffer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TriangleIndexDimensionBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

UINT TriangleIndexDimensionBuffer::GetIndexCount() const {
	return BaseDimensionBuffer::GetCapacity() * 3; //!< 三角形の面数 * 3
}

D3D12_INDEX_BUFFER_VIEW TriangleIndexDimensionBuffer::GetIndexBufferView() const {
	D3D12_INDEX_BUFFER_VIEW view = {};
	view.BufferLocation = BaseDimensionBuffer::GetGpuVirtualAddress();
	view.SizeInBytes    = static_cast<UINT>(BaseDimensionBuffer::GetByteSize());
	view.Format         = DXGI_FORMAT_R32_UINT;
	return view;
}

UINT* TriangleIndexDimensionBuffer::GetIndexData() {
	return reinterpret_cast<UINT*>(DimensionBuffer::GetData());
}

TriangleIndexDimensionBuffer TriangleIndexDimensionBuffer::Create(uint32_t faceCount, uint8_t frameCount) {
	return Graphics::Core::CreateDimensionBuffer<TriangleIndexDimension>(faceCount, frameCount);
}
