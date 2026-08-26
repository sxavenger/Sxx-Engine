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
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// ConstantBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief [ConstantBuffer] 定数Bufferクラス.
template <typename T>
class ConstantBuffer
	: public BaseDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ConstantBuffer() : BaseDimensionBuffer(sizeof(T)) {}
	~ConstantBuffer() = default;

	//* resource option *//

	//! @brief Resourceのデバッグ用の名前を設定する.
	void SetName(const std::wstring_view& name) const;

	//! @brief Resourceのデバッグ用の名前を設定する.
	void SetName(const std::string_view& name) const;

	//* data option *//

	//! @brief 指定したindexの要素を取得する.
	T& At();

	//! @brief 指定したindexの要素を取得する.
	const T& At() const;

	//* operator [copy] <ConstantBuffer> (delete) *//

	ConstantBuffer(const ConstantBuffer&)            = delete;
	ConstantBuffer& operator=(const ConstantBuffer&) = delete;

	//* operator [move] <ConstantBuffer> *//

	ConstantBuffer(ConstantBuffer&&) noexcept            = default;
	ConstantBuffer& operator=(ConstantBuffer&&) noexcept = default;

	//* static methods *//

	static ConstantBuffer Create(
		const Device& device, ResourceAllocator& allocator,
		uint8_t frameCount
	);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<T*> datas_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// ConstantBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void ConstantBuffer<T>::SetName(const std::wstring_view& name) const {
	BaseDimensionBuffer::SetName(std::format(L"<Constant Buffer> {}", name));
}

template <typename T>
inline void ConstantBuffer<T>::SetName(const std::string_view& name) const {
	BaseDimensionBuffer::SetName(std::format("<Constant Buffer> {}", name));
}

template <typename T>
inline T& ConstantBuffer<T>::At() {
	return *datas_[handle_.GetCurrentIndex()];
}

template <typename T>
inline const T& ConstantBuffer<T>::At() const {
	return *datas_[handle_.GetCurrentIndex()];
}

template <typename T>
inline ConstantBuffer<T> ConstantBuffer<T>::Create(
	const Device& device, ResourceAllocator& allocator,
	uint8_t frameCount) {

	ConstantBuffer<T> constant;

	//!< DimensionBufferの生成.
	BaseDimensionBuffer::CreateBuffer(constant, device, allocator, Category::Upload, 1, frameCount); //!< ConstantBufferは定数(1つの要素しか持たない).

	ResourceAllocator::Buffer& buffers = allocator.GetBuffer(constant.GetHandle());
	for (uint8_t i = 0; i < buffers.size(); ++i) {
		T* ptr = nullptr;
		buffers[i].Map(reinterpret_cast<void**>(&ptr));
		constant.datas_.emplace_back(ptr);
	}

	constant.BaseDimensionBuffer::SetName(L"<Constant Buffer>");
	return constant;
}

SXAVENGER_ENGINE_NAMESPACE_END
