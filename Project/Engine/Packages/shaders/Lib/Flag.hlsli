#pragma once

#ifndef FLAG_LIBRARY_HLSLI
#define FLAG_LIBRARY_HLSLI

////////////////////////////////////////////////////////////////////////////////////////////
// FlagMask structure
////////////////////////////////////////////////////////////////////////////////////////////
template <typename Bit, typename Mask>
struct FlagMask {

	//=========================================================================================
	// public variables
	//=========================================================================================

	Mask value;

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* create *//

	static FlagMask Create() { FlagMask result; result.value = (Mask)0; return result; }
	static FlagMask Create(Mask mask) { FlagMask result; result.value = mask; return result; }
	static FlagMask Create(Bit bit) { return Create((Mask)bit); }

	//* check mask functions *//

	bool Test(Mask mask) { return (value & mask) == mask; }
	bool Test(Bit bit) { return Test((Mask)bit); }
	bool Test(FlagMask other) { return Test(other.value); }

	bool Any(Mask mask) { return (value & mask) != 0; }
	bool Any(Bit bit) { return Any((Mask)bit); }
	bool Any(FlagMask other) { return Any(other.value); }

	//* mask option *//

	void Clear() { value = (Mask)0; }
	void Clear(Mask mask) { value &= ~mask; }
	void Clear(Bit bit) { Clear((Mask)bit); }
	void Clear(FlagMask other) { Clear(other.value); }

	void Inverse(Mask mask) { value ^= mask; }
	void Inverse(Bit bit) { Inverse((Mask)bit); }
	void Inverse(FlagMask other) { Inverse(other.value); }

	void Set(Mask mask, bool val) { value = val ? (value | mask) : (value & ~mask); }
	void Set(Bit bit, bool val) { Set((Mask)bit, val); }
	void Set(FlagMask other, bool val) { Set(other.value, val); }

	void Set(Mask mask) { Set(mask, true); }
	void Set(Bit bit) { Set(bit, true); }
	void Set(FlagMask other) { Set(other, true); }

	void Reset(Mask mask) { value &= ~mask; }
	void Reset(Bit bit) { Reset((Mask)bit); }
	void Reset(FlagMask other) { Reset(other.value); }

	//* operator [compare] *//

	bool operator==(Mask mask) { return value == mask; }
	bool operator==(Bit bit) { return value == (Mask)bit; }
	bool operator==(FlagMask other) { return value == other.value; }

	bool operator!=(Mask mask) { return value != mask; }
	bool operator!=(Bit bit) { return value != (Mask)bit; }
	bool operator!=(FlagMask other) { return value != other.value; }

	//* operator [or] *//

	FlagMask operator|(Mask mask) { return Create(value | mask); }
	FlagMask operator|(Bit bit) { return Create(value | (Mask)bit); }
	FlagMask operator|(FlagMask other) { return Create(value | other.value); }

	//* operator [and] *//

	FlagMask operator&(Mask mask) { return Create(value & mask); }
	FlagMask operator&(Bit bit) { return Create(value & (Mask)bit); }
	FlagMask operator&(FlagMask other) { return Create(value & other.value); }

	//* operator [xor] *//

	FlagMask operator^(Mask mask) { return Create(value ^ mask); }
	FlagMask operator^(Bit bit) { return Create(value ^ (Mask)bit); }
	FlagMask operator^(FlagMask other) { return Create(value ^ other.value); }

	//* cast *//

	Mask ToMask() { return value; }

	Bit ToBit() { return (Bit)value; }

};

////////////////////////////////////////////////////////////////////////////////////////////
// FlagEnum alias
////////////////////////////////////////////////////////////////////////////////////////////
template <typename Enum>
using FlagEnum = FlagMask<Enum, uint32_t>; //!< hlslのenum classはuint32_tを基底型とする.

#endif
