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

	constexpr FlagMask() : mask_(NULL) {}

	//* check mask functions *//

	constexpr bool Test(_Mask mask) const { return (mask_ & mask) == mask; }
	constexpr bool Test(_Bit bit) const { return Test(static_cast<_Mask>(bit)); }

	constexpr bool Any(_Mask mask) const { return (mask_ & mask) != 0; }
	constexpr bool Any(_Bit bit) const { return Any(static_cast<_Mask>(bit)); }

	//* mask option *//

	constexpr void Clear() { mask_ = NULL; }
	constexpr void Clear(_Mask mask) { mask_ &= ~mask; }

	constexpr void Inverse(_Mask mask) { mask_ ^= mask; }
	constexpr void Inverse(_Bit bit) { Inverse(static_cast<_Mask>(bit)); }

	constexpr void Set(_Mask mask) { mask_ |= mask; }
	constexpr void Set(_Bit bit) { Set(static_cast<_Mask>(bit)); }

	constexpr void Reset(_Mask mask) { mask_ &= ~mask; }
	constexpr void Reset(_Bit bit) { Reset(static_cast<_Mask>(bit)); }

	//* operator [assign] *//

	constexpr FlagMask(const FlagMask&) noexcept            = default;
	constexpr FlagMask& operator=(const FlagMask&) noexcept = default;

	constexpr FlagMask(_Mask mask) noexcept : mask_(mask) {}
	constexpr FlagMask& operator=(_Mask mask) noexcept { mask_ = mask; return *this; }

	constexpr FlagMask(_Bit bit) noexcept : mask_(static_cast<_Mask>(bit)) {}
	constexpr FlagMask& operator=(_Bit bit) noexcept { mask_ = static_cast<_Mask>(bit); return *this; }

	//* operator [compare] *//

	constexpr bool operator==(_Mask mask) const noexcept { return mask_ == mask; }
	constexpr bool operator==(_Bit bit) const noexcept { return mask_ == static_cast<_Mask>(bit); }
	constexpr bool operator==(FlagMask other) const noexcept { return mask_ == other.mask_; }

	constexpr bool operator!=(_Mask mask) const noexcept { return mask_ != mask; }
	constexpr bool operator!=(_Bit bit) const noexcept { return mask_ != static_cast<_Mask>(bit); }
	constexpr bool operator!=(FlagMask other) const noexcept { return mask_ != other.mask_; }

	//* operator [or] *//

	constexpr FlagMask& operator|=(_Mask mask) noexcept { mask_ |= mask; return *this; }
	constexpr FlagMask& operator|=(_Bit bit) noexcept { mask_ |= static_cast<_Mask>(bit); return *this; }
	constexpr FlagMask& operator|=(FlagMask other) noexcept { mask_ |= other.mask_; return *this; }

	constexpr FlagMask operator|(_Mask mask) const noexcept { return FlagMask(mask_ | mask); }
	constexpr FlagMask operator|(_Bit bit) const noexcept { return FlagMask(mask_ | static_cast<_Mask>(bit)); }
	constexpr FlagMask operator|(FlagMask other) const noexcept { return FlagMask(mask_ | other.mask_); }

	//* operator [and] *//

	constexpr FlagMask& operator&=(_Mask mask) noexcept { mask_ &= mask; return *this; }
	constexpr FlagMask& operator&=(_Bit bit) noexcept { mask_ &= static_cast<_Mask>(bit); return *this; }
	constexpr FlagMask& operator&=(FlagMask other) noexcept { mask_ &= other.mask_; return *this; }

	constexpr FlagMask operator&(_Mask mask) const noexcept { return FlagMask(mask_ & mask); }
	constexpr FlagMask operator&(_Bit bit) const noexcept { return FlagMask(mask_ & static_cast<_Mask>(bit)); }
	constexpr FlagMask operator&(FlagMask other) const noexcept { return FlagMask(mask_ & other.mask_); }

	//* operator [xor] *//

	constexpr FlagMask& operator^=(_Mask mask) noexcept { mask_ ^= mask; return *this; }
	constexpr FlagMask& operator^=(_Bit bit) noexcept { mask_ ^= static_cast<_Mask>(bit); return *this; }
	constexpr FlagMask& operator^=(FlagMask other) noexcept { mask_ ^= other.mask_; return *this; }

	constexpr FlagMask operator^(_Mask mask) const noexcept { return FlagMask(mask_ ^ mask); }
	constexpr FlagMask operator^(_Bit bit) const noexcept { return FlagMask(mask_ ^ static_cast<_Mask>(bit)); }
	constexpr FlagMask operator^(FlagMask other) const noexcept { return FlagMask(mask_ ^ other.mask_); }

	//* operator [cast] *//

	explicit constexpr operator _Mask() const { return mask_; }
	explicit constexpr operator _Bit() const { return static_cast<_Bit>(mask_); }


	//=========================================================================================
	// public variables
	//=========================================================================================

	_Mask mask_ = NULL;

};

////////////////////////////////////////////////////////////////////////////////////////////
// FlagEnum concept
////////////////////////////////////////////////////////////////////////////////////////////
template <Concept::Enum _Enum>
using FlagEnum = FlagMask<_Enum, std::underlying_type_t<_Enum>>;

//* enum classにビット演算子を定義するマクロ *//
#define ENUM_FLAG_OPERATORS(T) \
	inline constexpr T operator|(T a, T b) { \
		using U = std::underlying_type_t<T>; \
		return static_cast<T>(static_cast<U>(a) | static_cast<U>(b)); \
	} \
	inline constexpr T operator&(T a, T b) { \
		using U = std::underlying_type_t<T>; \
		return static_cast<T>(static_cast<U>(a) & static_cast<U>(b)); \
	} \
	inline constexpr T operator^(T a, T b) { \
		using U = std::underlying_type_t<T>; \
		return static_cast<T>(static_cast<U>(a) ^ static_cast<U>(b)); \
	} \
	inline constexpr T operator~(T a) { \
		using U = std::underlying_type_t<T>; \
		return static_cast<T>(~static_cast<U>(a)); \
	} \
