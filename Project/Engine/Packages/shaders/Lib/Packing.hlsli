#pragma once

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
		return float32_t(value) * kScale;
	}
	
	//! @brief int8_t(SNORM8)値をfloatに変換する.
	static float32_t DecodeSnorm8(uint32_t value) {
		static const float32_t kScale = 1.0f / 127.0f;
		int32_t sign_extended = int32_t(value << 24) >> 24; //!< 下位8bitを2の補数として符号拡張.
		return float32_t(sign_extended) * kScale;
	}
	
}