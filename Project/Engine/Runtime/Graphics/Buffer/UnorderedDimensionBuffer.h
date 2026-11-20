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
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// UnorderedDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief [UnorderedAccessBuffer] 1次元Bufferクラス.
template <typename T>
class UnorderedDimensionBuffer
	: public BaseDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	UnorderedDimensionBuffer() : BaseDimensionBuffer(sizeof(T)) {}
	~UnorderedDimensionBuffer() = default;

	//* resource option *//

	//! @brief Resourceのデバッグ用の名前を設定する.
	void SetName(const std::wstring_view& name) const;

	//! @brief Resourceのデバッグ用の名前を設定する.
	void SetName(const std::string_view& name) const;

	//* operator [copy] <DimensionBuffer> (delete) *//

	UnorderedDimensionBuffer(const UnorderedDimensionBuffer&)            = delete;
	UnorderedDimensionBuffer& operator=(const UnorderedDimensionBuffer&) = delete;

	//* operator [move] <DimensionBuffer> *//

	UnorderedDimensionBuffer(UnorderedDimensionBuffer&&) noexcept            = default;
	UnorderedDimensionBuffer& operator=(UnorderedDimensionBuffer&&) noexcept = default;

	//* static methods *//

	static UnorderedDimensionBuffer Create(
		const Device& device, ResourceAllocator& allocator,
		uint32_t capacity, uint8_t frameCount
	);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// UnorderedDimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void UnorderedDimensionBuffer<T>::SetName(const std::wstring_view& name) const {
	BaseDimensionBuffer::SetName(std::format(L"<Unordered Dimension Buffer> {}", name));
}

template <typename T>
inline void UnorderedDimensionBuffer<T>::SetName(const std::string_view& name) const {
	BaseDimensionBuffer::SetName(std::format("<Unordered Dimension Buffer> {}", name));
}

template <typename T>
inline UnorderedDimensionBuffer<T> UnorderedDimensionBuffer<T>::Create(
	const Device& device, ResourceAllocator& allocator,
	uint32_t capacity, uint8_t frameCount) {

	UnorderedDimensionBuffer<T> dimension;

	//!< DimensionBufferの作成.
	BaseDimensionBuffer::CreateBuffer(dimension, device, allocator, Category::Unordered, capacity, frameCount);

	dimension.BaseDimensionBuffer::SetName(L"<Unordered Dimension Buffer>");
	return dimension;
}

SXAVENGER_ENGINE_NAMESPACE_END


