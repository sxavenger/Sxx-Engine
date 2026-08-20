#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Traits/Concept.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FlagMask structure
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Bit, class _Mask>
struct FlagMask {
	//* maskを指定してフラグを管理するクラス
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr FlagMask() : value(NULL) {}

	//* check mask functions *//

	constexpr bool Test(_Mask mask) const { return (value & mask) == mask; }
	constexpr bool Test(_Bit bit) const { return Test(static_cast<_Mask>(bit)); }
	constexpr bool Test(FlagMask other) const { return Test(other.value); }

	constexpr bool Any(_Mask mask) const { return (value & mask) != 0; }
	constexpr bool Any(_Bit bit) const { return Any(static_cast<_Mask>(bit)); }
	constexpr bool Any(FlagMask other) const { return Any(other.value); }

	//* mask option *//

	constexpr void Clear() { value = NULL; }
	constexpr void Clear(_Mask mask) { value &= ~mask; }
	constexpr void Clear(_Bit bit) { Clear(static_cast<_Mask>(bit)); }
	constexpr void Clear(FlagMask other) { Clear(other.value); }

	constexpr void Inverse(_Mask mask) { value ^= mask; }
	constexpr void Inverse(_Bit bit) { Inverse(static_cast<_Mask>(bit)); }
	constexpr void Inverse(FlagMask other) { Inverse(other.value); }

	constexpr void Set(_Mask mask) { value |= mask; }
	constexpr void Set(_Bit bit) { Set(static_cast<_Mask>(bit)); }
	constexpr void Set(FlagMask other) { Set(other.value); }

	constexpr void Reset(_Mask mask) { value &= ~mask; }
	constexpr void Reset(_Bit bit) { Reset(static_cast<_Mask>(bit)); }
	constexpr void Reset(FlagMask other) { Reset(other.value); }

	//* operator [assign] *//

	constexpr FlagMask(const FlagMask&) noexcept            = default;
	constexpr FlagMask& operator=(const FlagMask&) noexcept = default;

	constexpr FlagMask(_Mask mask) noexcept : value(mask) {}
	constexpr FlagMask& operator=(_Mask mask) noexcept { value = mask; return *this; }

	constexpr FlagMask(_Bit bit) noexcept : value(static_cast<_Mask>(bit)) {}
	constexpr FlagMask& operator=(_Bit bit) noexcept { value = static_cast<_Mask>(bit); return *this; }

	//* operator [compare] *//

	constexpr bool operator==(_Mask mask) const noexcept { return value == mask; }
	constexpr bool operator==(_Bit bit) const noexcept { return value == static_cast<_Mask>(bit); }
	constexpr bool operator==(FlagMask other) const noexcept { return value == other.value; }

	constexpr bool operator!=(_Mask mask) const noexcept { return value != mask; }
	constexpr bool operator!=(_Bit bit) const noexcept { return value != static_cast<_Mask>(bit); }
	constexpr bool operator!=(FlagMask other) const noexcept { return value != other.value; }

	//* operator [or] *//

	constexpr FlagMask& operator|=(_Mask mask) noexcept { value |= mask; return *this; }
	constexpr FlagMask& operator|=(_Bit bit) noexcept { value |= static_cast<_Mask>(bit); return *this; }
	constexpr FlagMask& operator|=(FlagMask other) noexcept { value |= other.value; return *this; }

	constexpr FlagMask operator|(_Mask mask) const noexcept { return FlagMask(value | mask); }
	constexpr FlagMask operator|(_Bit bit) const noexcept { return FlagMask(value | static_cast<_Mask>(bit)); }
	constexpr FlagMask operator|(FlagMask other) const noexcept { return FlagMask(value | other.value); }

	//* operator [and] *//

	constexpr FlagMask& operator&=(_Mask mask) noexcept { value &= mask; return *this; }
	constexpr FlagMask& operator&=(_Bit bit) noexcept { value &= static_cast<_Mask>(bit); return *this; }
	constexpr FlagMask& operator&=(FlagMask other) noexcept { value &= other.value; return *this; }

	constexpr FlagMask operator&(_Mask mask) const noexcept { return FlagMask(value & mask); }
	constexpr FlagMask operator&(_Bit bit) const noexcept { return FlagMask(value & static_cast<_Mask>(bit)); }
	constexpr FlagMask operator&(FlagMask other) const noexcept { return FlagMask(value & other.value); }

	//* operator [xor] *//

	constexpr FlagMask& operator^=(_Mask mask) noexcept { value ^= mask; return *this; }
	constexpr FlagMask& operator^=(_Bit bit) noexcept { value ^= static_cast<_Mask>(bit); return *this; }
	constexpr FlagMask& operator^=(FlagMask other) noexcept { value ^= other.value; return *this; }

	constexpr FlagMask operator^(_Mask mask) const noexcept { return FlagMask(value ^ mask); }
	constexpr FlagMask operator^(_Bit bit) const noexcept { return FlagMask(value ^ static_cast<_Mask>(bit)); }
	constexpr FlagMask operator^(FlagMask other) const noexcept { return FlagMask(value ^ other.value); }

	//* operator [cast] *//

	explicit constexpr operator _Mask() const { return value; }
	explicit constexpr operator _Bit() const { return static_cast<_Bit>(value); }


	//=========================================================================================
	// public variables
	//=========================================================================================

	_Mask value = NULL;

};

////////////////////////////////////////////////////////////////////////////////////////////
// FlagEnum concept
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::Enum _Enum>
using FlagEnum = FlagMask<_Enum, std::underlying_type_t<_Enum>>;

//-----------------------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------------------
//* enum classにビット演算子を定義するマクロ *//
//! @brief enum class同士のビット演算をFlagEnum<T>として返す.
//! @note 列挙子として存在しない値がT型で作られることを防ぐため, 戻り値はFlagEnum<T>とする.
//! @note enum classと同じnamespace内で使用すること. (ADLで探索されるため)
#define ENUM_FLAG_OPERATORS(T) \
	inline constexpr FlagEnum<T> operator|(T a, T b) noexcept { \
		return FlagEnum<T>(a) | b; \
	} \
	inline constexpr FlagEnum<T> operator|(T a, FlagEnum<T> b) noexcept { \
		return b | a; \
	} \
	inline constexpr FlagEnum<T> operator&(T a, T b) noexcept { \
		return FlagEnum<T>(a) & b; \
	} \
	inline constexpr FlagEnum<T> operator&(T a, FlagEnum<T> b) noexcept { \
		return b & a; \
	} \
	inline constexpr FlagEnum<T> operator^(T a, T b) noexcept { \
		return FlagEnum<T>(a) ^ b; \
	} \
	inline constexpr FlagEnum<T> operator^(T a, FlagEnum<T> b) noexcept { \
		return b ^ a; \
	} \
	inline constexpr FlagEnum<T> operator~(T a) noexcept { \
		using U = std::underlying_type_t<T>; \
		return FlagEnum<T>(static_cast<U>(~static_cast<U>(a))); \
	} \
