#pragma once

// # reference
// [ACESlib.Utilities.ctl](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities.ctl)

//!< CTL 組み込み (IlmCtlMath: Chromaticities / RGBtoXYZ など) 相当の機能もここに置く

#ifndef ACES_UTILITIES_HLSLI
#define ACES_UTILITIES_HLSLI

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Numeric.hlsli"

//-----------------------------------------------------------------------------------------
// define
//-----------------------------------------------------------------------------------------

//!< limits - https://github.com/AcademySoftwareFoundation/openexr/blob/8bc3741131db146ad08a5b83af9e6e48f0e94a03/IlmBase/Half/half.h#L257-L261
#define ACES_HALF_MAX     65504.0
#define ACES_HALF_MIN     5.96046448e-08
#define ACES_HALF_NRM_MIN 6.10351562e-05

////////////////////////////////////////////////////////////////////////////////////////////
// ACES namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ACES {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Chromaticities structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 色度座標 (CIE 1931 xy)
	//! @reference https://github.com/ampas/CTL/blob/e280b6c4cf49726914260f1cf0cb2949d8db1027/lib/IlmCtlMath/CtlColorSpace.h#L126-L132
	struct Chromaticities {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float32_t2 red;
		float32_t2 green;
		float32_t2 blue;
		float32_t2 white;

		//=========================================================================================
		// public methods
		//=========================================================================================

		static Chromaticities Create(float32_t2 r, float32_t2 g, float32_t2 b, float32_t2 w) {
			Chromaticities chromaticities;
			chromaticities.red   = r;
			chromaticities.green = g;
			chromaticities.blue  = b;
			chromaticities.white = w;

			return chromaticities;
		}

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief 3x3行列の逆行列の計算
	float32_t3x3 Inverse3x3(float32_t3x3 m) {
		float a = m[0][0], b = m[0][1], c = m[0][2];
		float d = m[1][0], e = m[1][1], f = m[1][2];
		float g = m[2][0], h = m[2][1], i = m[2][2];

		float A =  (e * i - f * h);
		float B = -(d * i - f * g);
		float C =  (d * h - e * g);

		float det = a * A + b * B + c * C;
		float invDet = 1.0 / det;

		float3x3 r;
		r[0][0] = A * invDet;
		r[0][1] = -(b * i - c * h) * invDet;
		r[0][2] =  (b * f - c * e) * invDet;
		r[1][0] = B * invDet;
		r[1][1] =  (a * i - c * g) * invDet;
		r[1][2] = -(a * f - c * d) * invDet;
		r[2][0] = C * invDet;
		r[2][1] = -(a * h - b * g) * invDet;
		r[2][2] =  (a * e - b * d) * invDet;
		return r;
	}

	//! @brief 色度座標から RGB -> XYZ 行列を構築する
	//! @reference [RGBtoXYZ_f33(...)](https://github.com/ampas/CTL/blob/e280b6c4cf49726914260f1cf0cb2949d8db1027/lib/IlmCtlMath/CtlColorSpace.cpp#L77-L132)
	float32_t3x3 RGBtoXYZ(Chromaticities c, float32_t y) {

		// RGB = (1,1,1) すなわち白の X, Z
		float X = c.white.x * y / c.white.y;
		float Z = (1.0 - c.white.x - c.white.y) * y / c.white.y;

		// 各行のスケール係数
		float d = c.red.x   * (c.blue.y  - c.green.y) +
			c.blue.x  * (c.green.y - c.red.y) +
			c.green.x * (c.red.y   - c.blue.y);

		float Sr = (X * (c.blue.y - c.green.y) -
			c.green.x * (y * (c.blue.y  - 1.0) + c.blue.y  * (X + Z)) +
			c.blue.x  * (y * (c.green.y - 1.0) + c.green.y * (X + Z))) / d;

		float Sg = (X * (c.red.y - c.blue.y) +
			c.red.x  * (y * (c.blue.y - 1.0) + c.blue.y * (X + Z)) -
			c.blue.x * (y * (c.red.y  - 1.0) + c.red.y  * (X + Z))) / d;

		float Sb = (X * (c.green.y - c.red.y) -
			c.red.x   * (y * (c.green.y - 1.0) + c.green.y * (X + Z)) +
			c.green.x * (y * (c.red.y   - 1.0) + c.red.y   * (X + Z))) / d;

		float3x3 M;
		M[0][0] = Sr * c.red.x;
		M[0][1] = Sr * c.red.y;
		M[0][2] = Sr * (1.0 - c.red.x - c.red.y);

		M[1][0] = Sg * c.green.x;
		M[1][1] = Sg * c.green.y;
		M[1][2] = Sg * (1.0 - c.green.x - c.green.y);

		M[2][0] = Sb * c.blue.x;
		M[2][1] = Sb * c.blue.y;
		M[2][2] = Sb * (1.0 - c.blue.x - c.blue.y);

		return M;
	}

	//! @brief 色度座標から XYZ -> RGB 行列を構築する
	//! @reference [XYZtoRGB_f33(...)](https://github.com/ampas/CTL/blob/e280b6c4cf49726914260f1cf0cb2949d8db1027/lib/IlmCtlMath/CtlColorSpace.cpp#L135-L139)
	float32_t3x3 XYZtoRGB(Chromaticities c, float32_t y) {
		return Inverse3x3(RGBtoXYZ(c, y));
	}

}

#endif
