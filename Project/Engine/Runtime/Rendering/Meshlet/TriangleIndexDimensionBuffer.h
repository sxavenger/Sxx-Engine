#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/DimensionBuffer.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using TriangleIndexDimension = std::array<UINT, 3>; //!< 三角形プリミティブ用のインデックスバッファの要素型

////////////////////////////////////////////////////////////////////////////////////////////
// TriangleIndexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class TriangleIndexDimensionBuffer
	: public Graphics::DimensionBuffer<TriangleIndexDimension> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	TriangleIndexDimensionBuffer() : Graphics::DimensionBuffer<TriangleIndexDimension>() {}
	~TriangleIndexDimensionBuffer() = default;

	//* option *//
			
	//! @brief インデックス総数を取得する
	UINT GetIndexCount() const;

	//! @brief インデックスバッファビューを取得する
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

	//! @brief インデックスバッファのデータを取得する
	UINT* GetIndexData();

	//* operator [copy] <TriangleIndexDimensionBuffer> (delete) *//

	TriangleIndexDimensionBuffer(const TriangleIndexDimensionBuffer&)            = delete;
	TriangleIndexDimensionBuffer& operator=(const TriangleIndexDimensionBuffer&) = delete;

	//* operator [move] <TriangleIndexDimensionBuffer> *//

	TriangleIndexDimensionBuffer(TriangleIndexDimensionBuffer&&) noexcept            = default;
	TriangleIndexDimensionBuffer& operator=(TriangleIndexDimensionBuffer&&) noexcept = default;

	//* operator [move] <DimensionBuffer> *//

	TriangleIndexDimensionBuffer(Graphics::DimensionBuffer<TriangleIndexDimension>&& other) noexcept : Graphics::DimensionBuffer<TriangleIndexDimension>(std::move(other)) {}
	TriangleIndexDimensionBuffer& operator=(Graphics::DimensionBuffer<TriangleIndexDimension>&& other) noexcept { Graphics::DimensionBuffer<TriangleIndexDimension>::operator=(std::move(other)); return *this; }

	//* static methods *//

	//! @brief 三角形インデックスバッファを作成する
	static TriangleIndexDimensionBuffer Create(uint32_t faceCount, uint8_t frameCount);

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
