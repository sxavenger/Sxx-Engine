#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/DimensionBuffer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// VertexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class VertexDimensionBuffer
	: public Graphics::DimensionBuffer<T> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	VertexDimensionBuffer() : Graphics::DimensionBuffer<T>() {}
	~VertexDimensionBuffer() = default;

	//* option *//

	//! @brief 頂点バッファビューを取得する.
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

	//* operator [copy] <VertexDimensionBuffer> (delete) *//

	VertexDimensionBuffer(const VertexDimensionBuffer&)            = delete;
	VertexDimensionBuffer& operator=(const VertexDimensionBuffer&) = delete;

	//* operator [move] <VertexDimensionBuffer> *//

	VertexDimensionBuffer(VertexDimensionBuffer&&) noexcept            = default;
	VertexDimensionBuffer& operator=(VertexDimensionBuffer&&) noexcept = default;

	//* operator [move] <DimensionBuffer> *//

	VertexDimensionBuffer(Graphics::DimensionBuffer<T>&& other) noexcept : Graphics::DimensionBuffer<T>(std::move(other)) {}
	VertexDimensionBuffer& operator=(Graphics::DimensionBuffer<T>&& other) noexcept { Graphics::DimensionBuffer<T>::operator=(std::move(other)); return *this; }

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// VertexDimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline D3D12_VERTEX_BUFFER_VIEW VertexDimensionBuffer<T>::GetVertexBufferView() const {
	D3D12_VERTEX_BUFFER_VIEW view = {};
	view.BufferLocation = Graphics::BaseDimensionBuffer::GetGpuVirtualAddress();
	view.SizeInBytes    = static_cast<UINT>(Graphics::BaseDimensionBuffer::GetByteSize());
	view.StrideInBytes  = static_cast<UINT>(Graphics::BaseDimensionBuffer::GetStride());

	return view;
}

SXAVENGER_ENGINE_NAMESPACE_END
