#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <cstddef>

////////////////////////////////////////////////////////////////////////////////////////////
// ReferencePointer class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class ReferencePointer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//
	
	ReferencePointer() = default;

	//* operator [copy / move] <ReferencePointer> *//

	ReferencePointer(const ReferencePointer&)            = default;
	ReferencePointer& operator=(const ReferencePointer&) = default;

	ReferencePointer(ReferencePointer&&)            = default;
	ReferencePointer& operator=(ReferencePointer&&) = default;

	//* operator [copy] <T*> *//

	ReferencePointer(T* pointer) : pointer_(pointer) {}
	ReferencePointer& operator=(T* pointer) { pointer_ = pointer; return *this; }

	//* operator [copy] <ReferencePointer<U>> *//

	template <typename U> requires std::convertible_to<U*, T*>
	ReferencePointer(const ReferencePointer<U>& pointer) : pointer_(pointer.Get()) {}

	template <typename U> requires std::convertible_to<U*, T*>
	ReferencePointer& operator=(const ReferencePointer<U>& pointer) { pointer_ = pointer.Get(); return *this; }

	//* operator [copy] <U*> *//

	template <typename U> requires std::convertible_to<U*, T*>
	ReferencePointer(U* pointer) : pointer_(pointer) {}

	template <typename U> requires std::convertible_to<U*, T*>
	ReferencePointer& operator=(U* pointer) { pointer_ = pointer; return *this; }

	//* operator [copy] <std::nullptr_t> *//

	ReferencePointer(std::nullptr_t) : pointer_(nullptr) {}
	ReferencePointer& operator=(std::nullptr_t) { pointer_ = nullptr; return *this; }

	//* operator [comparison] <ReferencePointer> *//

	bool operator==(const ReferencePointer& other) const { return pointer_ == other.pointer_; }
	bool operator!=(const ReferencePointer& other) const { return pointer_ != other.pointer_; }

	//* operator [comparison] <T*> *//

	bool operator==(const T* pointer) const { return pointer_ == pointer; }
	bool operator!=(const T* pointer) const { return pointer_ != pointer; }

	//* operator [comparison] <std::nullptr_t> *//

	bool operator==(std::nullptr_t) const { return pointer_ == nullptr; }
	bool operator!=(std::nullptr_t) const { return pointer_ != nullptr; }

	//* operator [cast] <T*> *//

	operator T* const () { return pointer_; }
	operator const T* () const { return pointer_; }

	//* operator [cast] <bool> *//

	explicit operator bool() const { return pointer_ != nullptr; }

	//* operator [dereference] *//

	T& operator*();
	const T& operator*() const;

	//* operator [access] *//

	T* operator->();
	const T* operator->() const;

	//* pointer methods *//

	void Reset() { pointer_ = nullptr; }

	T* Get() noexcept { return pointer_; }
	T* Get() const noexcept { return pointer_; }

	T& GetRef();
	const T& GetRef() const;

	uintptr_t GetAddress() const { return reinterpret_cast<uintptr_t>(pointer_); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	T* pointer_ = nullptr;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ReferencePointer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
T& ReferencePointer<T>::operator*() {
	StreamLogger::Assert(pointer_ != nullptr, "ReferencePointer is null.");
	return *pointer_;
}

template <typename T>
const T& ReferencePointer<T>::operator*() const {
	StreamLogger::Assert(pointer_ != nullptr, "ReferencePointer is null.");
	return *pointer_;
}

template <typename T>
T* ReferencePointer<T>::operator->() {
	StreamLogger::Assert(pointer_ != nullptr, "ReferencePointer is null.");
	return pointer_;
}

template <typename T>
const T* ReferencePointer<T>::operator->() const {
	StreamLogger::Assert(pointer_ != nullptr, "ReferencePointer is null.");
	return pointer_;
}

template <typename T>
inline T& ReferencePointer<T>::GetRef() {
	StreamLogger::Assert(pointer_ != nullptr, "ReferencePointer is null.");
	return *pointer_;
}

template <typename T>
inline const T& ReferencePointer<T>::GetRef() const {
	StreamLogger::Assert(pointer_ != nullptr, "ReferencePointer is null.");
	return *pointer_;
}

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------

template <typename T>
using RefPtr = ReferencePointer<T>;
