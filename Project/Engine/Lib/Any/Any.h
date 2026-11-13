#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Reflection/TypeInfo.h>
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <any>

////////////////////////////////////////////////////////////////////////////////////////////
// Any structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Any final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Any() noexcept = default;

	//* any option *//

	bool HasValue() const noexcept { return data.has_value(); }

	TypeInfo GetType() const noexcept { return TypeInfo(data.type()); }

	template <typename T>
	T& Get();

	template <typename T>
	const T& Get() const;

	void Reset() noexcept { data.reset(); }

	//* operator [copy / move] <Any> *//

	Any(const Any&) noexcept            = default;
	Any& operator=(const Any&) noexcept = default;

	Any(Any&&) noexcept            = default;
	Any& operator=(Any&&) noexcept = default;

	//* operator [copy / move] <std::any> *//

	Any(const std::any& any) : data(any) {}
	Any& operator=(const std::any& any) { data = any; return *this; }

	Any(std::any&& any) noexcept : data(std::move(any)) {}
	Any& operator=(std::any&& any) noexcept { data = std::move(any); return *this; }

	//* operator [assignment] <T> *//

	template <typename T>
	Any(const T& value) : data(value) {}

	template <typename T>
	Any& operator=(const T& value) { data = value; return *this; }

	//* any option *//

	template <typename T>
	static Any Create(const T& value) noexcept { return Any(value); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::any data;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Any structure template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
T& Any::Get() {
	STREAM_ASSERT(GetType() == typeid(T), "any type mismatch.");
	return std::any_cast<T&>(data);
}

template <typename T>
const T& Any::Get() const {
	STREAM_ASSERT(GetType() == typeid(T), "any type mismatch.");
	return std::any_cast<const T&>(data);
}
