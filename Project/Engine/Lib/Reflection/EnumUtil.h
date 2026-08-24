#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Traits/Concept.h>

//* externals
#include <magic_enum.hpp>
#include <magic_enum_format.hpp>

//* c++
#include <concepts>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// EnumUtil class
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::Enum Enum>
class EnumUtil final {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Underlying = std::underlying_type_t<Enum>; //!< Enumの基になる整数型.

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief Enumの値の数を取得する.
	constexpr static const Underlying GetCount();

	//! @brief Enumの値を基になる整数型にキャストする.
	constexpr static const Underlying Cast(Enum value);

	//! @brief Enumの値に対応する名前を取得する.
	constexpr static std::string_view GetName(Enum value);

	//! @brief Enumのインデックスに対応する名前を取得する.
	constexpr static std::string_view GetName(Underlying index);

	//! @brief Enumの値と名前のペアの配列を取得する.
	//! @return <Enum, std::string_view>の配列.
	constexpr static auto GetEntries();

	//! @brief Enumのインデックスに対応するEnumの値を取得する.
	constexpr static Enum GetEnum(Underlying index);

	//! @brief Enumの名前に対応するEnumの値が存在するかを取得する.
	constexpr static bool Contains(const std::string_view& name);

	//! @brief Enumの名前に対応するEnumの値を取得する.
	//! @throw nameに対応するEnumの値が存在しない場合. 
	constexpr static Enum GetEnum(const std::string_view& name);

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// EnumUtil class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Concept::Enum Enum>
inline constexpr const EnumUtil<Enum>::Underlying EnumUtil<Enum>::GetCount() {
	return static_cast<Underlying>(magic_enum::enum_count<Enum>());
}

template <Concept::Enum Enum>
inline constexpr const EnumUtil<Enum>::Underlying EnumUtil<Enum>::Cast(Enum value) {
	return static_cast<Underlying>(value);
}

template <Concept::Enum Enum>
inline constexpr std::string_view EnumUtil<Enum>::GetName(Enum value) {
	return magic_enum::enum_name(value);
}

template <Concept::Enum Enum>
inline constexpr std::string_view EnumUtil<Enum>::GetName(EnumUtil<Enum>::Underlying index) {
	auto name = magic_enum::enum_names<Enum>();
	return index < name.size() ? name[index] : std::string_view{};
}

template <Concept::Enum Enum>
inline constexpr auto EnumUtil<Enum>::GetEntries() {
	return magic_enum::enum_entries<Enum>();
}

template <Concept::Enum Enum>
inline constexpr Enum EnumUtil<Enum>::GetEnum(EnumUtil<Enum>::Underlying index) {
	auto values = magic_enum::enum_values<Enum>();
	return index < values.size() ? static_cast<Enum>(values[index]) : static_cast<Enum>(0);
}

template <Concept::Enum Enum>
inline constexpr bool EnumUtil<Enum>::Contains(const std::string_view& name) {
	return magic_enum::enum_contains<Enum>(name);
}

template <Concept::Enum Enum>
inline constexpr Enum EnumUtil<Enum>::GetEnum(const std::string_view& name) {
	STREAM_ASSERT(EnumUtil<Enum>::Contains(name), "EnumUtil | invalid enum name. name: {}", name);
	return magic_enum::enum_cast<Enum>(name).value();
}
