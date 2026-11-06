#pragma once
//!< FXAAの定義関係

// # reference
// [Fxaa_3.11.hlsli](https://gist.github.com/jwoolston/c96c22473110c9924d831cebb5f2cdb4)

//-----------------------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------------------

//! @brief 早期終了を有効にする場合は[FXAA_EARLY_EXIT 1]を定義する. 0の場合は早期終了無効.
#ifndef FXAA_EARLY_EXIT
#define FXAA_EARLY_EXIT 1
#endif

//! @brief AA不要なピクセルをdiscardする場合は[FXAA_DISCARD 1]を定義する. 0の場合は元の色を返す.
#ifndef FXAA_DISCARD
#define FXAA_DISCARD 0
#endif

//* --- FXAA Quality Presets --- *//

//! @brief 品質プリセット.
//! 10 ~ 15 : 中程度のディザ (10 = 最速, 15 = 最高品質)
//! 20 ~ 29 : 低ディザ, 高負荷 (20 = 最速, 29 = 最高品質)
//! 39      : ディザなし, 超高負荷
//! @note 12 = 既定値, 13 = FXAA 3.9と同程度の速度でより高品質, 23 = FXAA 3.9に最も近い見た目
#ifndef FXAA_QUALITY__PRESET
#define FXAA_QUALITY__PRESET 12
#endif

//* medium dither presets *//

#if (FXAA_QUALITY__PRESET == 10)
#define FXAA_QUALITY__PS 3
#define FXAA_QUALITY__P0 1.5
#define FXAA_QUALITY__P1 3.0
#define FXAA_QUALITY__P2 12.0
#endif

#if (FXAA_QUALITY__PRESET == 11)
#define FXAA_QUALITY__PS 4
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 3.0
#define FXAA_QUALITY__P3 12.0
#endif

#if (FXAA_QUALITY__PRESET == 12)
#define FXAA_QUALITY__PS 5
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 2.0
#define FXAA_QUALITY__P3 4.0
#define FXAA_QUALITY__P4 12.0
#endif

#if (FXAA_QUALITY__PRESET == 13)
#define FXAA_QUALITY__PS 6
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 2.0
#define FXAA_QUALITY__P3 2.0
#define FXAA_QUALITY__P4 4.0
#define FXAA_QUALITY__P5 12.0
#endif

#if (FXAA_QUALITY__PRESET == 14)
#define FXAA_QUALITY__PS 7
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 2.0
#define FXAA_QUALITY__P3 2.0
#define FXAA_QUALITY__P4 2.0
#define FXAA_QUALITY__P5 4.0
#define FXAA_QUALITY__P6 12.0
#endif

#if (FXAA_QUALITY__PRESET == 15)
#define FXAA_QUALITY__PS 8
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 2.0
#define FXAA_QUALITY__P3 2.0
#define FXAA_QUALITY__P4 2.0
#define FXAA_QUALITY__P5 2.0
#define FXAA_QUALITY__P6 4.0
#define FXAA_QUALITY__P7 12.0
#endif

//* low dither presets *//

#if (FXAA_QUALITY__PRESET == 20)
#define FXAA_QUALITY__PS 3
#define FXAA_QUALITY__P0 1.5
#define FXAA_QUALITY__P1 2.0
#define FXAA_QUALITY__P2 8.0
#endif

#if (FXAA_QUALITY__PRESET == 21)
#define FXAA_QUALITY__PS 4
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 2.0
#define FXAA_QUALITY__P3 8.0
#endif

#if (FXAA_QUALITY__PRESET == 22)
#define FXAA_QUALITY__PS 5
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 2.0
#define FXAA_QUALITY__P3 2.0
#define FXAA_QUALITY__P4 8.0
#endif

#if (FXAA_QUALITY__PRESET == 23)
#define FXAA_QUALITY__PS 6
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 2.0
#define FXAA_QUALITY__P3 2.0
#define FXAA_QUALITY__P4 2.0
#define FXAA_QUALITY__P5 8.0
#endif

#if (FXAA_QUALITY__PRESET == 24)
#define FXAA_QUALITY__PS 7
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 2.0
#define FXAA_QUALITY__P3 2.0
#define FXAA_QUALITY__P4 2.0
#define FXAA_QUALITY__P5 3.0
#define FXAA_QUALITY__P6 8.0
#endif

#if (FXAA_QUALITY__PRESET == 25)
#define FXAA_QUALITY__PS 8
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 2.0
#define FXAA_QUALITY__P3 2.0
#define FXAA_QUALITY__P4 2.0
#define FXAA_QUALITY__P5 2.0
#define FXAA_QUALITY__P6 4.0
#define FXAA_QUALITY__P7 8.0
#endif

#if (FXAA_QUALITY__PRESET == 26)
#define FXAA_QUALITY__PS 9
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 2.0
#define FXAA_QUALITY__P3 2.0
#define FXAA_QUALITY__P4 2.0
#define FXAA_QUALITY__P5 2.0
#define FXAA_QUALITY__P6 2.0
#define FXAA_QUALITY__P7 4.0
#define FXAA_QUALITY__P8 8.0
#endif

#if (FXAA_QUALITY__PRESET == 27)
#define FXAA_QUALITY__PS 10
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 2.0
#define FXAA_QUALITY__P3 2.0
#define FXAA_QUALITY__P4 2.0
#define FXAA_QUALITY__P5 2.0
#define FXAA_QUALITY__P6 2.0
#define FXAA_QUALITY__P7 2.0
#define FXAA_QUALITY__P8 4.0
#define FXAA_QUALITY__P9 8.0
#endif

#if (FXAA_QUALITY__PRESET == 28)
#define FXAA_QUALITY__PS 11
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 2.0
#define FXAA_QUALITY__P3 2.0
#define FXAA_QUALITY__P4 2.0
#define FXAA_QUALITY__P5 2.0
#define FXAA_QUALITY__P6 2.0
#define FXAA_QUALITY__P7 2.0
#define FXAA_QUALITY__P8 2.0
#define FXAA_QUALITY__P9 4.0
#define FXAA_QUALITY__P10 8.0
#endif

#if (FXAA_QUALITY__PRESET == 29)
#define FXAA_QUALITY__PS 12
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.5
#define FXAA_QUALITY__P2 2.0
#define FXAA_QUALITY__P3 2.0
#define FXAA_QUALITY__P4 2.0
#define FXAA_QUALITY__P5 2.0
#define FXAA_QUALITY__P6 2.0
#define FXAA_QUALITY__P7 2.0
#define FXAA_QUALITY__P8 2.0
#define FXAA_QUALITY__P9 2.0
#define FXAA_QUALITY__P10 4.0
#define FXAA_QUALITY__P11 8.0
#endif

//* extreme quality preset *//

#if (FXAA_QUALITY__PRESET == 39)
#define FXAA_QUALITY__PS 12
#define FXAA_QUALITY__P0 1.0
#define FXAA_QUALITY__P1 1.0
#define FXAA_QUALITY__P2 1.0
#define FXAA_QUALITY__P3 1.0
#define FXAA_QUALITY__P4 1.0
#define FXAA_QUALITY__P5 1.5
#define FXAA_QUALITY__P6 2.0
#define FXAA_QUALITY__P7 2.0
#define FXAA_QUALITY__P8 2.0
#define FXAA_QUALITY__P9 2.0
#define FXAA_QUALITY__P10 4.0
#define FXAA_QUALITY__P11 8.0
#endif

#ifndef FXAA_QUALITY__PS
#error "FXAA_QUALITY__PRESET is invalid."
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// FXAA namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace FXAA {

	//-----------------------------------------------------------------------------------------
	// typedef
	//-----------------------------------------------------------------------------------------

	//!< boolean
	typedef bool Bool;

	//!< float-point
	typedef float32_t  Float;
	typedef float32_t2 Float2;
	typedef float32_t3 Float3;
	typedef float32_t4 Float4;

	//!< integral
	typedef int32_t   Int;
	typedef int32_t2  Int2;

}
