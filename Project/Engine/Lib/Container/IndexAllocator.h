#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <concepts>
#include <optional>
#include <queue>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////////////////
// IndexAllocator class
////////////////////////////////////////////////////////////////////////////////////////////
template <std::integral T>
class IndexAllocator {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	IndexAllocator() = default;
	IndexAllocator(T capacity) : capacity_(capacity) {}

	//* allocator option *//

	void Reset();

	//! @brief indexの最大数を設定.
	void Capacity(T capacity);

	//! @brief indexを取得. 解放されたindexがある場合は、解放されたindexを優先して返す.
	//! @throw capacityを超えるindexを返すとき. 
	T Allocate();

	//! @brief indexを解放し、再利用可能なindexとしてキューに追加.
	void Free(T index);

	//! @brief indexの最大数を取得.
	T GetCapacity() const { return capacity_; }

	//! @brief 現在使用中のindexの数を取得.
	T GetUsedCount() const { return current_ - static_cast<T>(free_.size()); }

private:

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* parameter *//

	T capacity_ = 0;

	//* runtime state *//

	T current_ = 0;
	std::queue<T> free_;

	//* thread *//

	std::mutex mutex_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// IndexAllocator class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <std::integral T>
inline void IndexAllocator<T>::Reset() {
	capacity_ = NULL;
	current_  = 0;
	std::queue<T>().swap(free_); //!< キューを空にする.
}

template <std::integral T>
inline void IndexAllocator<T>::Capacity(T capacity) {
	capacity_ = std::max(capacity_, capacity); //!< すでにcapacityが設定されている場合は、より大きい値を設定する.
}

template <std::integral T>
T IndexAllocator<T>::Allocate() {
	std::unique_lock<std::mutex> lock(mutex_); //!< スレッドセーフにするためにロックする.

	if (!free_.empty()) {
		//!< 解放されたindexがある場合は、キューから取得する.
		T index = free_.front();
		free_.pop();

		return index;
	}

	STREAM_ASSERT(current_ < capacity_, "index allocator capacity over. current: {}, capacity: {}", current_, capacity_);
	return current_++; //!< 解放されたindexがない場合は、currentを返し、currentをインクリメントする.
}

template <std::integral T>
void IndexAllocator<T>::Free(T index) {
	STREAM_ASSERT(index < current_, "index is out of range. index: {}, current: {}.", index, current_); //!< 解放するindexがcurrentの範囲内にあることを確認する.

	std::unique_lock<std::mutex> lock(mutex_); //!< スレッドセーフにするためにロックする.
	free_.emplace(index); //!< 解放されたindexをキューに追加する.
}
