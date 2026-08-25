#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "BaseDimensionBuffer.h"

//* engine
#include <Runtime/Foundation.hpp>

//* c++
#include <vector>
#include <span>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// DimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief [ConstantBuffer/StructuredBuffer] 1次元Bufferクラス.
template <typename T>
class DimensionBuffer final
	: public BaseDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	DimensionBuffer() : BaseDimensionBuffer(sizeof(T)) {}
	~DimensionBuffer() = default;

	//* data option *//

	//! @brief 指定したindexの要素を取得する.
	T& At(size_t index);

	//! @brief 指定したindexの要素を取得する.
	const T& At(size_t index) const;

	//* operator [copy] <DimensionBuffer> (delete) *//

	DimensionBuffer(const DimensionBuffer&)            = delete;
	DimensionBuffer& operator=(const DimensionBuffer&) = delete;

	//* operator [move] <DimensionBuffer> *//

	DimensionBuffer(DimensionBuffer&&) noexcept            = default;
	DimensionBuffer& operator=(DimensionBuffer&&) noexcept = default;

	//* static methods *//

	static DimensionBuffer Create(
		const Device& device, ResourceAllocator& allocator,
		uint32_t capacity, uint8_t frameCount
	);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<std::span<T>> datas_;

	//=========================================================================================
	// private methods
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// DimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline T& DimensionBuffer<T>::At(size_t index) {
	STREAM_ASSERT(index < capacity_, "index is out of range.");
	return datas_[handle_.GetCurrentIndex()][index];
}

template <typename T>
inline const T& DimensionBuffer<T>::At(size_t index) const {
	STREAM_ASSERT(index < capacity_, "index is out of range.");
	return datas_[handle_.GetCurrentIndex()][index];
}

template <typename T>
inline DimensionBuffer<T> DimensionBuffer<T>::Create(
	const Device& device, ResourceAllocator& allocator,
	uint32_t capacity, uint8_t frameCount) {

	
	DimensionBuffer<T> dimension;

	//!< DimensionBufferの作成.
	BaseDimensionBuffer::CreateBuffer(dimension, device, allocator, Category::Upload, capacity, frameCount);

	//!< Resourceのマッピング.
	ResourceAllocator::Buffer& buffers = allocator.GetBuffer(dimension.handle_.GetHandle());
	for (uint8_t i = 0; i < buffers.size(); ++i) {
		T* ptr = nullptr;
		buffers[i].Map(reinterpret_cast<void**>(&ptr));
		dimension.datas_.emplace_back(std::span<T>(ptr, capacity));
	}

	dimension.SetName(L"Dimension Buffer");
	return dimension;
}

SXAVENGER_ENGINE_NAMESPACE_END
