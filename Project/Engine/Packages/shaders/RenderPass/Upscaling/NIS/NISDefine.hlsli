#pragma once
//!< NISScalerの定義関係

// # reference
// [NIS_Scaler.h](https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h)

//-----------------------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------------------

//!< NVScaler有効時は[NIS_SCALER 1]を定義する. 0の場合はNVSharpenのみ.
// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L113-L116
#ifndef NIS_SCALER
#define NIS_SCALER 1 //!< デフォルトはNVScaler有効
#endif

//!< HDRModes
// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L118-L125
#define NIS_HDR_MODE_NONE   0
#define NIS_HDR_MODE_LINEAR 1
#define NIS_HDR_MODE_PQ     2
#ifndef NIS_HDR_MODE
#define NIS_HDR_MODE NIS_HDR_MODE_NONE
#endif

//!< 指定したViewportの範囲内でのみNISを適用する場合は[NIS_VIEWPORT_SUPPORT 1]を定義する. 0の場合はViewport無効.
// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L127-L130
#ifndef NIS_VIEWPORT_SUPPORT
#define NIS_VIEWPORT_SUPPORT 0
#endif

//!< Half precision
// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L147-L150
#ifndef NIS_USE_HALF_PRECISION
#define NIS_USE_HALF_PRECISION 1
#endif

//!< NV12入力を使用する場合は[NIS_NV12_SUPPORT 1]を定義する.
// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L100
#ifndef NIS_NV12_SUPPORT
#define NIS_NV12_SUPPORT 0
#endif

//!< 出力をclampする場合は[NIS_CLAMP_OUTPUT 1]を定義する.
// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L101
#ifndef NIS_CLAMP_OUTPUT
#define NIS_CLAMP_OUTPUT 0
#endif

//!< Texture gather
// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L234-L237
#ifndef NIS_TEXTURE_GATHER
#define NIS_TEXTURE_GATHER 0
#endif

//!< NIS Scaling
// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L239-L241
#define NIS_SCALE_INT   1
#define NIS_SCALE_FLOAT 1.0f

//!< NIS output clamp
// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L243-L252
#if NIS_CLAMP_OUTPUT
#if NIS_HDR_MODE == NIS_HDR_MODE_LINEAR
#define NVCLAMP(x) (clamp(x, 0.0f, 12.5f))
#else
#define NVCLAMP(x) (saturate(x))
#endif
#else
#define NVCLAMP(x) (x)
#endif

// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L239-L242
#define NIS_SCALE_INT   1
#define NIS_SCALE_FLOAT 1.0f

#if NIS_SCALER
// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L327-L349

#define NIS_BLOCK_WIDTH       32
#define NIS_BLOCK_HEIGHT      24
#define NIS_THREAD_GROUP_SIZE 256

#define NIS_PHASE_COUNT  64
#define NIS_FILTER_SIZE  6
#define NIS_SUPPORT_SIZE 6
#define NIS_PAD_SIZE     NIS_SUPPORT_SIZE

#define NIS_TILE_PITCH (NIS_BLOCK_WIDTH + NIS_PAD_SIZE)
#define NIS_TILE_SIZE  (NIS_TILE_PITCH * (NIS_BLOCK_HEIGHT + NIS_PAD_SIZE))

#define NIS_EDGE_MAP_PITCH (NIS_BLOCK_WIDTH + 2)
#define NIS_EDGE_MAP_SIZE  (NIS_EDGE_MAP_PITCH * (NIS_BLOCK_HEIGHT + 2))

#else
// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L817-L829

#define NIS_BLOCK_WIDTH       32
#define NIS_BLOCK_HEIGHT      32
#define NIS_THREAD_GROUP_SIZE 256

#define NIS_SUPPORT_SIZE 5
#define NIS_NIS_NUM_PIXEL_X (NIS_BLOCK_WIDTH + NIS_SUPPORT_SIZE + 1)
#define NIS_NIS_NUM_PIXEL_Y (NIS_BLOCK_HEIGHT + NIS_SUPPORT_SIZE + 1)

#endif

////////////////////////////////////////////////////////////////////////////////////////////
// NIS namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace NIS {

	//-----------------------------------------------------------------------------------------
	// typedef
	//-----------------------------------------------------------------------------------------
	// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L152-L180

	//!< floating-point
	typedef float32_t  NVF;
	typedef float32_t2 NVF2;
	typedef float32_t3 NVF3;
	typedef float32_t4 NVF4;

	//!< half precision
#if NIS_USE_HALF_PRECISION //!< 有効の場合はfloat16_tを使用する. 無効の場合はfloat32_tを使用する.
	typedef float16_t  NVH;
	typedef float16_t2 NVH2;
	typedef float16_t3 NVH3;
	typedef float16_t4 NVH4;
#else
	typedef float32_t  NVH;
	typedef float32_t2 NVH2;
	typedef float32_t3 NVH3;
	typedef float32_t4 NVH4;
#endif

	//!< integral
	typedef int32_t   NVI;
	typedef int32_t2  NVI2;
	typedef uint32_t  NVU;
	typedef uint32_t2 NVU2;

	//!< bool
	typedef bool NVB;

	//-----------------------------------------------------------------------------------------
	// constant variables
	//-----------------------------------------------------------------------------------------

	// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L125
	static const NVF kHDRCompressionFactor = 0.282842712;

}
