#pragma once

#ifndef PACKING_LIBRARY_HLSLI
#define PACKING_LIBRARY_HLSLI

////////////////////////////////////////////////////////////////////////////////////////////
// Packing namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Packing {

	//=========================================================================================
	// methods
	//=========================================================================================

	//! @brief uint16_t(UNORM16)値をfloatに変換する.
	float32_t DecodeUnorm16(uint32_t value) {
		static const float32_t kScale = 1.0f / 65535.0f;
		return float32_t(value & 0xFFFF) * kScale; //!< 下位16bitを使用する.
	}

	//! @brief floatをint8_t(SNORM8)値に変換する. (下位8bitに格納)
	static uint32_t EncodeSnorm8(float32_t value) {
		float32_t scaled  = value * 127.0f;
		int32_t   rounded = int32_t(floor(abs(scaled) + 0.5f)) * (scaled < 0.0f ? -1 : 1);
		return uint32_t(clamp(rounded, -128, 127)) & 0xFF;
	}

	//! @brief int8_t(SNORM8)値をfloatに変換する.
	static float32_t DecodeSnorm8(uint32_t value) {
		static const float32_t kScale = 1.0f / 127.0f;
		int32_t sign_extended = int32_t(value << 24) >> 24; //!< 下位8bitを2の補数として符号拡張.
		return float32_t(sign_extended) * kScale;
	}

}

#endif
