#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Pointer/ReferencePointer.h>

//* c++
#include <typeinfo>
#include <string_view>

////////////////////////////////////////////////////////////////////////////////////////////
// TypeInfo structure
////////////////////////////////////////////////////////////////////////////////////////////
struct TypeInfo final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr TypeInfo() noexcept = default;

	//* type info option *//

	size_t GetHashCode() const noexcept;

	std::string_view GetName() const noexcept;

	//* operator [copy / move] <TypeInfo> *//

	constexpr TypeInfo(const TypeInfo&) noexcept            = default;
	constexpr TypeInfo& operator=(const TypeInfo&) noexcept = default;

	constexpr TypeInfo(TypeInfo&&) noexcept            = default;
	constexpr TypeInfo& operator=(TypeInfo&&) noexcept = default;

	//* operator [copy] <std::type_info*> *//

	TypeInfo(const std::type_info& info) noexcept : info_(&info) {}
	TypeInfo& operator=(const std::type_info& info) noexcept { info_ = &info; return *this; }

	TypeInfo(const std::type_info* info) noexcept : info_(info) {}
	TypeInfo& operator=(const std::type_info* info) noexcept { info_ = info; return *this; }

	//* operator [comparison] <TypeInfo> *//

	bool operator==(const TypeInfo& other) const noexcept { return info_ == other.info_; }
	bool operator!=(const TypeInfo& other) const noexcept { return info_ != other.info_; }

	//* operator [comparison] <std::nullptr_t> *//

	bool operator==(std::nullptr_t) const noexcept { return info_ == nullptr; }
	bool operator!=(std::nullptr_t) const noexcept { return info_ != nullptr; }

	//* type info option *//

	template <typename T>
	static TypeInfo GetType() noexcept { return typeid(T); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	RefPtr<const std::type_info> info_ = nullptr;

};

////////////////////////////////////////////////////////////////////////////////////////////
// std::hash<TypeInfo> specialization
////////////////////////////////////////////////////////////////////////////////////////////
template <>
struct std::hash<TypeInfo> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	size_t operator()(const TypeInfo& typeInfo) const noexcept {
		return typeInfo.GetHashCode();
	}

};
