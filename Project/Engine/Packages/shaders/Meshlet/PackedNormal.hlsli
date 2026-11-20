#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include "../Lib/Packing.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// PackedNormal structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PackedNormal {

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint32_t packed;

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief float32_t3値として展開する.
	float32_t3 Unpack3() {
		return float32_t3(
			Packing::DecodeSnorm8(packed >> 0),
			Packing::DecodeSnorm8(packed >> 8),
			Packing::DecodeSnorm8(packed >> 16)
		);
	}

	//! @brief float32_t4値として展開する.
	float32_t4 Unpack4() {
		return float32_t4(
			Packing::DecodeSnorm8(packed >> 0),
			Packing::DecodeSnorm8(packed >> 8),
			Packing::DecodeSnorm8(packed >> 16),
			Packing::DecodeSnorm8(packed >> 24)
		);
	}

	//! @brief float32_t3値をPackedNormalに変換する.
	//! @note w成分は1.0として格納する.
	static PackedNormal Pack3(float32_t3 value) {
		PackedNormal packed;
		packed.packed
			= (Packing::EncodeSnorm8(value.x) << 0)
			| (Packing::EncodeSnorm8(value.y) << 8)
			| (Packing::EncodeSnorm8(value.z) << 16)
			| (Packing::EncodeSnorm8(1.0)     << 24);
		return packed;
	}

	//! @brief float32_t4値をPackedNormalに変換する.
	static PackedNormal Pack4(float32_t4 value) {
		PackedNormal packed;
		packed.packed
			= (Packing::EncodeSnorm8(value.x) << 0)
			| (Packing::EncodeSnorm8(value.y) << 8)
			| (Packing::EncodeSnorm8(value.z) << 16)
			| (Packing::EncodeSnorm8(value.w) << 24);
		return packed;
	}

};
