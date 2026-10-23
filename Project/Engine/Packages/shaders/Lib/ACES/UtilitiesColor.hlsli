#pragma once

// # reference
// [ACESlib.Utilities_Color.ctl](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl)

#ifndef ACES_UTILITIES_COLOR_HLSLI
#define ACES_UTILITIES_COLOR_HLSLI

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

	// --- Chromaticities --- //

	//! @brief Academy Primaries 0 (i.e. "ACES" Primaries from SMPTE ST2065-1)
	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L17-L23
	static const Chromaticities AP0 = Chromaticities::Create(
		float32_t2(0.73470,  0.26530), //!< red
		float32_t2(0.00000,  1.00000), //!< green
		float32_t2(0.00010, -0.07700), //!< blue
		float32_t2(0.32168,  0.33767)  //!< white
	);

	//! @brief Academy Primaries 1
	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L25-L31
	static const Chromaticities AP1 = Chromaticities::Create(
		float32_t2(0.713,   0.293),  //!< red
		float32_t2(0.165,   0.830),  //!< green
		float32_t2(0.128,   0.044),  //!< blue
		float32_t2(0.32168, 0.33767) //!< white
	);

	//! @brief limiting primaries and white point <Rec.709 Primaries / D65 White>
	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L33-L39
	static const Chromaticities REC709_PRI = Chromaticities::Create(
		float32_t2(0.64000, 0.33000), //!< red
		float32_t2(0.30000, 0.60000), //!< green
		float32_t2(0.15000, 0.06000), //!< blue
		float32_t2(0.31270, 0.32900)  //!< white
	);

	//! @brief display primaries and white point <P3-D65 Primaries / D60 White>
	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L41-L47
	static const Chromaticities P3D60_PRI = Chromaticities::Create(
		float32_t2(0.68000, 0.32000), //!< red
		float32_t2(0.26500, 0.69000), //!< green
		float32_t2(0.15000, 0.06000),   //!< blue
		float32_t2(0.32168, 0.33767)  //!< white
	);

	//! @brief limiting primaries and white point <Rec.2020 Primaries / D65 White>
	//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L73-L79
	static const Chromaticities REC2020_PRI = Chromaticities::Create(
		float32_t2(0.70800, 0.29200), //!< red
		float32_t2(0.17000, 0.79700), //!< green
		float32_t2(0.13100, 0.04600), //!< blue
		float32_t2(0.31270, 0.32900)  //!< white
	);

	//!< Chromatic Adaptation constants - https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L232-L242
	static const float32_t3x3 CONE_RESP_MAT_BRADFORD = float3x3(
		0.89510,  -0.75020, 0.03890,
		0.26640,  1.71350,  -0.06850,
		-0.16140, 0.03670,  1.02960
	);

	static const float32_t3x3 CONE_RESP_MAT_CAT02 = float3x3(
		0.73280,  -0.70360, 0.00300,
		0.42960,  1.69750,  0.01360,
		-0.16240, 0.00610,  0.98340
	);

	//!< constants from SMPTE ST 2084 - 2014 - https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L501-L508
	static const float32_t pq_m1 = 0.1593017578125; // ( 2610.0 / 4096.0 ) / 4.0;
	static const float32_t pq_m2 = 78.84375;        // ( 2523.0 / 4096.0 ) * 128.0;
	static const float32_t pq_c1 = 0.8359375;       // 3424.0 / 4096.0 or pq_c3 - pq_c2 + 1.0;
	static const float32_t pq_c2 = 18.8515625;      // ( 2413.0 / 4096.0 ) * 32.0;
	static const float32_t pq_c3 = 18.6875;         // ( 2392.0 / 4096.0 ) * 32.0;

	static const float32_t pq_C  = 10000.0;

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	// --- XYZ <-> xyY --- //

	//! @reference [XYZ_2_xyY(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L164-L174)
	float32_t3 XYZ2xyY(float32_t3 XYZ) {
		float32_t divisor = XYZ.x + XYZ.y + XYZ.z;

		if (divisor == 0.0) {
			divisor = 1e-10;
		}

		return float3(XYZ.x / divisor, XYZ.y / divisor, XYZ.y);
	}

	//! @reference [xyY_2_XYZ(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L176-L184)
	float32_t3 xyY2XYZ(float32_t3 xyY) {
		return float32_t3(
			xyY.x * xyY.z / max(xyY.y, 1e-10),
			xyY.z,
			(1.0 - xyY.x - xyY.y) * xyY.z / max(xyY.y, 1e-10)
		);
	}

	// --- RGB から派生量への変換 --- //

	//! @reference [rgb_2_hue(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L188-L202)
	//! @note CTL は無彩色 (R = G = B) で FLT_NAN を返すが, ここでは 0 を返す
	float32_t rgb2hue(float32_t3 rgb) {

		// RGB から幾何学的な色相角 (度, 0-360) を求める.

		if (rgb.x == rgb.y && rgb.y == rgb.z) {
			return 0.0; //!< CTL: FLT_NAN (RGB が等しい場合は色相が未定義)
		}

		float32_t hue = (180.0 / Numeric::kPi) * atan2(sqrt(3.0) * (rgb.y - rgb.z), 2.0 * rgb.x - rgb.y - rgb.z);

		if (hue < 0.0) {
			hue += 360.0;
		}

		return hue;
	}

	//! @reference [rgb_2_yc(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L204-L226)
	float32_t rgb2yc(float32_t3 rgb, float32_t ycRadiusWeight = 1.75) {

		// RGB を輝度の代理量 YC (~ Y + K * Chroma) に変換する.
		// ycRadiusWeight = 1 -> CMY 原色の YC が同値の無彩色と等しい, 2 -> RGB 原色の YC が等しい.

		const float32_t r = rgb.r;
		const float32_t g = rgb.g;
		const float32_t b = rgb.b;

		const float32_t chroma = sqrt(b * (b - g) + g * (g - r) + r * (r - b));

		return (b + g + r + ycRadiusWeight * chroma) / 3.0;
	}

	// --- 白色点の順応 (Von Kries / Bradford) --- //

	//! @reference [calculate_cat_matrix(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L244-L275)
	float32_t3x3 CalculateCatMatrix(float32_t2 src_xy, float32_t2 des_xy, float32_t3x3 coneRespMat = CONE_RESP_MAT_BRADFORD) {

		const float32_t3 src_xyY = float3(src_xy.x, src_xy.y, 1.0);
		const float32_t3 des_xyY = float3(des_xy.x, des_xy.y, 1.0);

		float32_t3 src_XYZ = xyY2XYZ(src_xyY);
		float32_t3 des_XYZ = xyY2XYZ(des_xyY);

		float32_t3 src_coneResp = mul(src_XYZ, coneRespMat);
		float32_t3 des_coneResp = mul(des_XYZ, coneRespMat);


		float32_t3x3 vkMat = float3x3(
			des_coneResp.x / src_coneResp.x, 0.0, 0.0,
			0.0, des_coneResp.y / src_coneResp.y, 0.0,
			0.0, 0.0, des_coneResp.z / src_coneResp.z
		);

		return mul(coneRespMat, mul(vkMat, Inverse3x3(coneRespMat)));
	}

	//! @reference [calculate_rgb_to_rgb_matrix(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L279-L317)
	float32_t3x3 CalculateRGBToRGBMatrix(Chromaticities SOURCE_PRIMARIES, Chromaticities DEST_PRIMARIES, float32_t3x3 coneRespMat = CONE_RESP_MAT_BRADFORD) {

		const float32_t3x3 RGBtoXYZ_MAT = RGBtoXYZ(SOURCE_PRIMARIES, 1.0);
		const float32_t3x3 CAT          = CalculateCatMatrix(SOURCE_PRIMARIES.white, DEST_PRIMARIES.white);
		const float32_t3x3 XYZtoRGB_MAT = XYZtoRGB(DEST_PRIMARIES, 1.0);

		return mul(RGBtoXYZ_MAT, mul(CAT, XYZtoRGB_MAT));
	}

	// --- 輝度ベースの彩度調整行列 --- //

	//! @reference [calc_sat_adjust_matrix(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L321-L345)
	float32_t3x3 CalcSatAdjustMatrix(float32_t sat, float32_t3 rgb2Y) {

		float3x3 M;
		M[0][0] = (1.0 - sat) * rgb2Y.x + sat;
		M[1][0] = (1.0 - sat) * rgb2Y.x;
		M[2][0] = (1.0 - sat) * rgb2Y.x;

		M[0][1] = (1.0 - sat) * rgb2Y.y;
		M[1][1] = (1.0 - sat) * rgb2Y.y + sat;
		M[2][1] = (1.0 - sat) * rgb2Y.y;

		M[0][2] = (1.0 - sat) * rgb2Y.z;
		M[1][2] = (1.0 - sat) * rgb2Y.z;
		M[2][2] = (1.0 - sat) * rgb2Y.z + sat;

		return transpose(M);
	}

	// --- モニタカーブ --- //

	//! @brief Forward monitor curve (コード値 -> リニア)
	//! @reference [moncurve_f(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L353-L364)
	float32_t MoncurveFwd(float32_t x, float32_t gamma, float32_t offs) {

		const float32_t fs = ((gamma - 1.0) / offs) * pow(offs * gamma / ((gamma - 1.0) * (1.0 + offs)), gamma);
		const float32_t xb = offs / (gamma - 1.0);

		if (x >= xb) {
			return pow((x + offs) / (1.0 + offs), gamma);
		}

		return fs * x;
	}

	//! @brief Reverse monitor curve (リニア -> コード値)
	//! @reference [moncurve_r(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L366-L377)
	float32_t MoncurveRev(float32_t y, float32_t gamma, float32_t offs) {

		const float32_t yb = pow(offs * gamma / ((gamma - 1.0) * (1.0 + offs)), gamma);
		const float32_t rs = pow((gamma - 1.0) / offs, gamma - 1.0) * pow((1.0 + offs) / gamma, gamma);

		if (y >= yb) {
			return (1.0 + offs) * pow(y, 1.0 / gamma) - offs;
		}

		return y * rs;
	}

	//! @reference [moncurve_f_f3(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L379-L386)
	float32_t3 MoncurveFwd(float32_t3 x, float32_t gamma, float32_t offs) {
		return float32_t3(
			MoncurveFwd(x.x, gamma, offs),
			MoncurveFwd(x.y, gamma, offs),
			MoncurveFwd(x.z, gamma, offs)
		);
	}

	//! @reference [moncurve_r_f3(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L388-L395)
	float32_t3 MoncurveRev(float32_t3 y, float32_t gamma, float32_t offs) {
		return float32_t3(
			MoncurveRev(y.x, gamma, offs),
			MoncurveRev(y.y, gamma, offs),
			MoncurveRev(y.z, gamma, offs)
		);
	}

	// --- BT.1886 --- //

	//! @reference [bt1886_f(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L397-L405)
	float32_t BT1886Fwd(float32_t V, float32_t gamma, float32_t Lw, float32_t Lb) {
		// The reference EOTF specified in Rec. ITU-R BT.1886. L = a(max[(V+b),0])^g

		const float32_t a = pow(pow(Lw, 1.0 / gamma) - pow(Lb, 1.0 / gamma), gamma);
		const float32_t b = pow(Lb, 1.0 / gamma) / (pow(Lw, 1.0 / gamma) - pow(Lb, 1.0 / gamma));
		return a * pow(max(V + b, 0.0), gamma);
	}

	//! @reference [bt1886_r(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L407-L415)
	float32_t BT1886Rev(float32_t L, float32_t gamma, float32_t Lw, float32_t Lb) {
		// The reference EOTF specified in Rec. ITU-R BT.1886. V = (L/a)^(1/g) - b

		const float32_t a = pow(pow(Lw, 1.0 / gamma) - pow(Lb, 1.0 / gamma), gamma);
		const float32_t b = pow(Lb, 1.0 / gamma) / (pow(Lw, 1.0 / gamma) - pow(Lb, 1.0 / gamma));
		return pow(max(L / a, 0.0), 1.0 / gamma) - b;
	}

	//! @reference [bt1886_f_f3(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L417-L424)
	float32_t3 BT1886Fwd(float32_t3 V, float32_t gamma, float32_t Lw, float32_t Lb) {
		return float32_t3(
			BT1886Fwd(V.x, gamma, Lw, Lb),
			BT1886Fwd(V.y, gamma, Lw, Lb),
			BT1886Fwd(V.z, gamma, Lw, Lb)
		);
	}

	//! @reference [bt1886_r_f3(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L426-L433)
	float32_t3 BT1886Rev(float32_t3 L, float32_t gamma, float32_t Lw, float32_t Lb) {
		return float32_t3(
			BT1886Rev(L.x, gamma, Lw, Lb),
			BT1886Rev(L.y, gamma, Lw, Lb),
			BT1886Rev(L.z, gamma, Lw, Lb)
		);
	}

	// --- SMPTE ST 2084 (PQ) --- //

	//! @reference [ST2084_2_Y(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L514-L525)
	float32_t ST2084ToY(float32_t N) {

		// PQ コード値 (0-1, full range) -> リニア (cd/m^2).

		N = max(N, 0.0); //!< pow(...)が負値で未定義なため追加

		float32_t Np = pow(N, 1.0 / pq_m2);
		float32_t L = Np - pq_c1;
		L = max(L, 0.0);
		L = L / ( pq_c2 - pq_c3 * Np );
		L = pow(L, 1.0 / pq_m1);
		return L * pq_C; // returns cd/m^2
	}

	//! @reference [Y_2_ST2084(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L531-L541)
	float32_t YToST2084(float32_t C) {

		// リニア (cd/m^2) -> PQ コード値 (0-1, full range).

		C = max(C, 0.0); //!< pow(...)が負値で未定義なため追加

		const float32_t L  = C / pq_C;
		const float32_t Lm = pow(L, pq_m1);
		const float32_t N  = (pq_c1 + pq_c2 * Lm) / (1.0 + pq_c3 * Lm);
		return pow(N, pq_m2);
	}

	//! @reference [Y_2_ST2084_f3(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L543-L553)
	float32_t3 YToST2084(float32_t3 C) {
		return float32_t3(
			YToST2084(C.x),
			YToST2084(C.y),
			YToST2084(C.z)
		);
	}

	//! @reference [ST2084_2_Y_f3(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L555-L565)
	float32_t3 ST2084ToY(float32_t3 N) {
		return float32_t3(
			ST2084ToY(N.x),
			ST2084ToY(N.y),
			ST2084ToY(N.z)
		);
	}

	// --- HLG (ITU-R BT.2100 / 1000 nits) --- //

	//! @reference [ST2084_2_HLG_1000nits_f3(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L569-L621)
	float32_t3 ST2084ToHLG1000nits(float32_t3 PQ) {

		float32_t3 displayLinear = ST2084ToY(PQ);

		// HLG Inverse EOTF (i.e. HLG inverse OOTF followed by the HLG OETF)
		// HLG Inverse OOTF (display linear to scene linear)
		float32_t Y_d = 0.2627 * displayLinear.x + 0.6780 * displayLinear.y + 0.0593 * displayLinear.z;
		const float32_t L_w   = 1000.0;
		const float32_t L_b   = 0.0;
		const float32_t alpha = (L_w - L_b);
		const float32_t beta  = L_b;
		const float32_t gamma = 1.2;

		float32_t3 sceneLinear = float32_t3(0.0, 0.0, 0.0);
		if (Y_d > 0.0) {
			sceneLinear = pow((Y_d - beta) / alpha, (1.0 - gamma) / gamma) * ((displayLinear - beta) /alpha);
		}

		// HLG OETF (scene linear to non-linear signal value)
		const float32_t a = 0.17883277;
		const float32_t b = 0.28466892; // 1.-4.*a;
		const float32_t c = 0.55991073; // 0.5-a*log(4.*a);

		float32_t3 HLG = float32_t3(0.0, 0.0, 0.0);
		[unroll]
		for (uint32_t i = 0; i < 3; ++i) {
			if (sceneLinear[i] <= 1.0 / 12.0) {
				HLG[i] = sqrt(3.0 * sceneLinear[i]);

			} else {
				HLG[i] = a * log(12.0 * sceneLinear[i] - b) + c;
			}
		}

		return HLG;
	}

	//! @reference [HLG_2_ST2084_1000nits_f3(...)](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.Utilities_Color.ctl#L625-L668)
	float32_t3 HLGToST20841000nits(float32_t3 HLG) {

		const float32_t a = 0.17883277;
		const float32_t b = 0.28466892; // 1.-4.*a;
		const float32_t c = 0.55991073; // 0.5-a*log(4.*a);

		const float32_t L_w   = 1000.0;
		const float32_t L_b   = 0.0;
		const float32_t alpha = (L_w - L_b);
		const float32_t beta  = L_b;
		const float32_t gamma = 1.2;

		float32_t3 sceneLinear = float32_t3(0.0, 0.0, 0.0);
		[unroll]
		for (uint32_t i = 0; i < 3; ++i) {
			if (HLG[i] >= 0. && HLG[i] <= 0.5) {
				sceneLinear[i] = pow(HLG[i], 2.0) / 3.0;

			} else {
				sceneLinear[i] = (exp((HLG[i] - c) / a) + b) / 12.0;
			}
		}

		float32_t Y_s = 0.2627 * sceneLinear.x + 0.6780 * sceneLinear.y + 0.0593 * sceneLinear.z;

		// Scene-linear to display-linear
		float32_t3 displayLinear = alpha * pow(Y_s, gamma - 1.0) * sceneLinear + beta;

		// ST.2084 Inverse EOTF
		return YToST2084(displayLinear);
	}

}

#endif
