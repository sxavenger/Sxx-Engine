#pragma once

// # reference
// [ACESlib.Tonescales.ctl](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Tonescales.ctl)

#ifndef ACES_TONESCALES_HLSLI
#define ACES_TONESCALES_HLSLI

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Utilities.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// ACES namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ACES {

	//=========================================================================================
	// constant variables
	//=========================================================================================

	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Tonescales.ctl#L6-L11
	static const float32_t3x3 SPLINE_M = float32_t3x3(
		0.5, -1.0, 0.5,
		-1.0, 1.0, 0.5,
		0.5,  0.0, 0.0
	);

	//-----------------------------------------------------------------------------------------
	// SplineMapPoint using
	//-----------------------------------------------------------------------------------------
	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Tonescales.ctl#L15-L19
	using SplineMapPoint = float32_t2;

	////////////////////////////////////////////////////////////////////////////////////////////
	// SegmentedSplineParams_c5 structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Tonescales.ctl#L21-L30
	struct SegmentedSplineParams_c5 {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float32_t coefsLow[6];   // coefs for B-spline between minPoint and midPoint (units of log luminance)
		float32_t coefsHigh[6];  // coefs for B-spline between midPoint and maxPoint (units of log luminance)
		SplineMapPoint minPoint; // {luminance, luminance} linear extension below this
		SplineMapPoint midPoint; // {luminance, luminance}
		SplineMapPoint maxPoint; // {luminance, luminance} linear extension above this
		float32_t slopeLow;      // log-log slope of low linear extension
		float32_t slopeHigh;     // log-log slope of high linear extension

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// SegmentedSplineParams_c9 structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Tonescales.ctl#L32-L41
	struct SegmentedSplineParams_c9 {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float32_t coefsLow[10];  // coefs for B-spline between minPoint and midPoint (units of log luminance)
		float32_t coefsHigh[10]; // coefs for B-spline between midPoint and maxPoint (units of log luminance)
		SplineMapPoint minPoint; // {luminance, luminance} linear extension below this
		SplineMapPoint midPoint; // {luminance, luminance}
		SplineMapPoint maxPoint; // {luminance, luminance} linear extension above this
		float32_t slopeLow;      // log-log slope of low linear extension
		float32_t slopeHigh;     // log-log slope of high linear extension

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// intermediate functions
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @reference [segmented_spline_c5_fwd(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Tonescales.ctl#L58-L153)
	float32_t SegmentedSplineC5Fwd(float32_t x, SegmentedSplineParams_c5 C) {
		const int32_t N_KNOTS_LOW  = 4;
		const int32_t N_KNOTS_HIGH = 4;

		// Check for negatives or zero before taking the log. If negative or zero,
		// set to HALF_MIN.
		const float32_t logx = log10(max(x, ACES_HALF_MIN));

		float32_t logy;
		if (logx <= log10(C.minPoint.x)) {
			logy = logx * C.slopeLow + (log10(C.minPoint.y) - C.slopeLow * log10(C.minPoint.x));

		} else if ((logx > log10(C.minPoint.x)) && (logx < log10(C.midPoint.x))) {

			const float32_t knot_coord = (N_KNOTS_LOW - 1) * (logx - log10(C.minPoint.x)) / (log10(C.midPoint.x) - log10(C.minPoint.x));
			const int32_t   j          = (int32_t)knot_coord;
			const float32_t t          = knot_coord - j;

			const float32_t3 cf        = float32_t3(C.coefsLow[j], C.coefsLow[j + 1], C.coefsLow[j + 2]);
			const float32_t3 monomials = float32_t3(t * t, t, 1.0);
			logy = dot(monomials, mul(cf, SPLINE_M));

		} else if ((logx >= log10(C.midPoint.x)) && (logx < log10(C.maxPoint.x))) {
			const float32_t knot_coord = (N_KNOTS_HIGH - 1) * (logx - log10(C.midPoint.x)) / (log10(C.maxPoint.x) - log10(C.midPoint.x));
			const int32_t   j          = (int32_t)knot_coord;
			const float32_t t          = knot_coord - j;

			const float32_t3 cf        = float32_t3(C.coefsHigh[j], C.coefsHigh[j + 1], C.coefsHigh[j + 2]);
			const float32_t3 monomials = float32_t3(t * t, t, 1.0);
			logy = dot(monomials, mul(cf, SPLINE_M));

		} else /* if (logIn >= log10(C.maxPoint.x)) */ {
			logy = logx * C.slopeHigh + (log10(C.maxPoint.y) - C.slopeHigh * log10(C.maxPoint.x));
		}

		return pow(10.0, logy);
	}

	//! @reference [segmented_spline_c9_fwd(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Tonescales.ctl#L317-L412)
	float32_t SegmentedSplineC9Fwd(float32_t x, SegmentedSplineParams_c9 C) {

		const int32_t N_KNOTS_LOW  = 8;
		const int32_t N_KNOTS_HIGH = 8;

		// Check for negatives or zero before taking the log. If negative or zero,
		// set to HALF_MIN.
		const float32_t logx = log10(max(x, ACES_HALF_MIN));

		float32_t logy;
		if (logx <= log10(C.minPoint.x)) {
			logy = logx * C.slopeLow + (log10(C.minPoint.y) - C.slopeLow * log10(C.minPoint.x));

		} else if ((logx > log10(C.minPoint.x)) && (logx < log10(C.midPoint.x))) {

			const float32_t knot_coord = (N_KNOTS_LOW - 1) * (logx - log10(C.minPoint.x)) / (log10(C.midPoint.x) - log10(C.minPoint.x));
			const int32_t j            = (int32_t)knot_coord;
			const float32_t t          = knot_coord - j;

			const float32_t3 cf        = float32_t3(C.coefsLow[j], C.coefsLow[j + 1], C.coefsLow[j + 2]);
			const float32_t3 monomials = float32_t3(t * t, t, 1.0);
			logy = dot(monomials, mul(cf, SPLINE_M));

		} else if ((logx >= log10(C.midPoint.x)) && (logx < log10(C.maxPoint.x))) {

			const float32_t knot_coord = (N_KNOTS_HIGH - 1) * (logx - log10(C.midPoint.x)) / (log10(C.maxPoint.x) - log10(C.midPoint.x));
			const int32_t j          = (int32_t)knot_coord;
			const float32_t t          = knot_coord - j;

			const float32_t3 cf        = float32_t3(C.coefsHigh[j], C.coefsHigh[j + 1], C.coefsHigh[j + 2]);
			const float32_t3 monomials = float32_t3(t * t, t, 1.0);
			logy = dot(monomials, mul(cf, SPLINE_M));

		} else /* if (logIn >= log10(C.maxPoint.x)) */ {
			logy = logx * C.slopeHigh + (log10(C.maxPoint.y) - C.slopeHigh * log10(C.maxPoint.x));
		}

		return pow(10.0, logy);
	}

	float32_t3 SegmentedSplineC5Fwd(float32_t3 x, SegmentedSplineParams_c5 C) {
		return float32_t3(
			SegmentedSplineC5Fwd(x.r, C),
			SegmentedSplineC5Fwd(x.g, C),
			SegmentedSplineC5Fwd(x.b, C)
		);
	}

	float32_t3 SegmentedSplineC9Fwd(float32_t3 x, SegmentedSplineParams_c9 C) {
		return float32_t3(
			SegmentedSplineC9Fwd(x.r, C),
			SegmentedSplineC9Fwd(x.g, C),
			SegmentedSplineC9Fwd(x.b, C)
		);
	}

	//=========================================================================================
	// constant variables
	//=========================================================================================

	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Tonescales.ctl#L44-L55
	static const SegmentedSplineParams_c5 RRT_PARAMS = {
		{ -4.0000000000, -4.0000000000, -3.1573765773, -0.4852499958, 1.8477324706, 1.8477324706 }, // coefsLow[6]
		{ -0.7185482425,  2.0810307172,  3.6681241237,  4.0000000000, 4.0000000000, 4.0000000000 }, // coefsHigh[6]
		{ 0.18 * pow(2.0, -15),     0.0001 }, // minPoint
		{ 0.18,                     4.8 },    // midPoint
		{ 0.18 * pow(2.0,  18), 10000.0 }, // maxPoint
		0.0, // slopeLow
		0.0  // slopeHigh
	};

	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Tonescales.ctl#L251-L262
	static const SegmentedSplineParams_c9 ODT_48nits = {
		{ -1.6989700043, -1.6989700043, -1.4779000000, -1.2291000000, -0.8648000000, -0.4480000000, 0.0051800000, 0.4511080334, 0.9113744414, 0.9113744414 }, // coefsLow[10]
		{  0.5154386965,  0.8470437783,  1.1358000000,  1.3802000000,  1.5197000000,  1.5985000000, 1.6467000000, 1.6746091357, 1.6878733390, 1.6878733390 }, // coefsHigh[10]
		{ SegmentedSplineC5Fwd(0.18 * pow(2.0, -6.5), RRT_PARAMS),  0.02 }, // minPoint
		{ SegmentedSplineC5Fwd(0.18,                  RRT_PARAMS),  4.8  }, // midPoint
		{ SegmentedSplineC5Fwd(0.18 * pow(2.0,  6.5), RRT_PARAMS), 48.0  }, // maxPoint
		0.0,  // slopeLow
		0.04  // slopeHigh
	};

	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Tonescales.ctl#L264-L275
	static const SegmentedSplineParams_c9 ODT_1000nits = {
		{ -4.9706219331, -3.0293780669, -2.1262, -1.5105, -1.0578, -0.4668, 0.11938, 0.7088134201, 1.2911865799, 1.2911865799 }, // coefsLow[10]
		{  0.8089132070,  1.1910867930,  1.5683,  1.9483,  2.3083,  2.6384, 2.8595,  2.9872608805, 3.0127391195, 3.0127391195 }, // coefsHigh[10]
		{ SegmentedSplineC5Fwd(0.18 * pow(2.0, -12.0), RRT_PARAMS),    0.0001 }, // minPoint
		{ SegmentedSplineC5Fwd(0.18,                   RRT_PARAMS),   10.0    }, // midPoint
		{ SegmentedSplineC5Fwd(0.18 * pow(2.0,  10.0), RRT_PARAMS), 1000.0    }, // maxPoint
		3.0,  // slopeLow
		0.06  // slopeHigh
	};

	static const SegmentedSplineParams_c9 ODT_2000nits = {
		{ -4.9706219331, -3.0293780669, -2.1262000000, -1.5105000000, -1.0578000000, -0.4668000000, 0.1193800000, 0.7088134201, 1.2911865799, 1.2911865799 }, // coefsLow[10]
		{  0.8019952042,  1.1980047958,  1.5943000000,  1.9973000000,  2.3783000000,  2.7684000000, 3.0515000000, 3.2746293562, 3.3274306351, 3.3274306351 }, // coefsHigh[10]
		{ SegmentedSplineC5Fwd(0.18 * pow(2.0, -12.0), RRT_PARAMS),    0.0001 }, // minPoint
		{ SegmentedSplineC5Fwd(0.18,                   RRT_PARAMS),   10.0    }, // midPoint
		{ SegmentedSplineC5Fwd(0.18 * pow(2.0,  11.0), RRT_PARAMS), 2000.0    }, // maxPoint
		3.0,  // slopeLow
		0.12  // slopeHigh
	};

}

#endif
