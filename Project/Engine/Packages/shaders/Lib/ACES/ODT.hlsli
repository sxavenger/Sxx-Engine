#pragma once

#ifndef ACES_ODT_HLSLI
#define ACES_ODT_HLSLI

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Tonescales.hlsli"
#include "ODTCommon.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// ACES namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ACES {

	//=========================================================================================
	// constant variables
	//=========================================================================================

	//!< https://github.com/aces-aswf/aces-core/blob/ed0c686a2067ee1daf60178fecc88633a0af26dc/transforms/ctl/odt/hdr_st2084/ODT.Academy.Rec2020_ST2084_1000nits.ctl#L75-L76
	static const float32_t ODT_HDR_BLACK_OFFSET = pow(10.0, -4.4550166483);

	////////////////////////////////////////////////////////////////////////////////////////////
	// ODT enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class ODT : uint32_t {
		sRGB_100nits_dim,        //!< https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/odt/sRGB/ODT.Academy.sRGB_100nits_dim.ctl
		Rec2020_ST2084_1000nits, //!< https://github.com/aces-aswf/aces-core/blob/ed0c686a2067ee1daf60178fecc88633a0af26dc/transforms/ctl/odt/hdr_st2084/ODT.Academy.Rec2020_ST2084_1000nits.ctl
		P3D60_ST2084_2000nits,   //!< https://github.com/aces-aswf/aces-core/blob/ed0c686a2067ee1daf60178fecc88633a0af26dc/transforms/ctl/odt/hdr_st2084/ODT.Academy.P3D60_ST2084_2000nits.ctl
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// EOTF enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @reference [ACESlib.OutputTransforms.ctl](https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/lib/ACESlib.OutputTransforms.ctl#L176-L183)
	enum class EOTF : uint32_t {
		ST2084,
		BT1886,
		sRGB,
		Gamma_2_6,
		Linear,
		HLG
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// ODTParams structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ODTParams {

		// tonescale       ODT_48nits / ODT_1000nits / ODT_2000nits
		// sdrCinemaToVideo true : Y_2_linCV (48 nits 基準の正規化) + dark -> dim surround + ODT_SAT
		//                        (sRGB ODT. 出力は 0-1 にクリップ)
		//                  false: 黒オフセットの減算 (HDR ODT. 出力は 0 以上にクリップ)
		// applyCAT        D60 -> D65 の CAT をかけるか (P3-D60 はかけない)
		// XYZ_to_display  XYZ -> 表示原色 (CTL の XYZ_2_DISPLAY_PRI_MAT)
		// Y_MIN / Y_MAX   表示の黒 / 白の輝度 (cd/m^2). ODT の出力 linearCV は Y_MIN .. Y_MAX を 0 .. 1 にした相対値で,
		//                 EOTF の符号化 (ACES1_EOTFInv) がこの 2 つを使う (ACESlib.OutputTransforms.ctl の
		//                 outputTransform() の Y_MIN / Y_MAX と同じ意味).
		//                 Y_MAX は各 ODT の Summary にある表示白の輝度. Y_MIN はどの ODT も 0
		//                 (sRGB ODT は CINEMA_BLACK を, HDR ODT は黒オフセットを引いて黒をコード値 0 に合わせているため).
		// eotf            その ODT 本来の伝達関数 (ACES1_EOTF_*). CTL の ODT と同じ出力になるのはこれを渡したとき

		//=========================================================================================
		// public variables
		//=========================================================================================

		SegmentedSplineParams_c9 tonescale;
		bool sdrCinemaToVideo;
		bool applyCAT;
		float32_t3x3 XYZ_to_display;
		float32_t Y_MIN;
		float32_t Y_MAX;
		EOTF eotf;

		//=========================================================================================
		// public methods
		//=========================================================================================

		static ODTParams Init(ODT odt) {

			ODTParams p;

			switch (odt) {
				case ODT::Rec2020_ST2084_1000nits:
					p.tonescale        = ODT_1000nits;
					p.sdrCinemaToVideo = false;
					p.applyCAT         = true;
					p.XYZ_to_display   = XYZtoRGB(REC2020_PRI, 1.0);
					p.Y_MIN            = 0.0;
					p.Y_MAX            = 1000.0;
					p.eotf             = EOTF::ST2084;
				break;

				case ODT::P3D60_ST2084_2000nits:
					p.tonescale        = ODT_2000nits;
					p.sdrCinemaToVideo = false;
					p.applyCAT         = false; // 表示の白が D60 なので CAT をかけない
					p.XYZ_to_display   = XYZtoRGB(P3D60_PRI, 1.0);
					p.Y_MIN            = 0.0;
					p.Y_MAX            = 2000.0;
					p.eotf             = EOTF::ST2084;
				break;

				case ODT::sRGB_100nits_dim:
					p.tonescale        = ODT_48nits;
					p.sdrCinemaToVideo = true;
					p.applyCAT         = true;
					p.XYZ_to_display   = XYZtoRGB(REC709_PRI, 1.0);
					p.Y_MIN            = 0.0;
					p.Y_MAX            = 100.0;  // linearCV の 1.0 が表示の白 (100 cd/m^2)
					p.eotf             = EOTF::sRGB;
				break;
			}

			return p;
		}

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief ODT main function | OCES (AP0) -> 表示原色の linearCV
	//! @param oces OCES
	//! @param p ODTParams
	//! @return linearCV
	float32_t3 ODTFwd(float32_t3 oces, ODTParams p) {

		// OCES to RGB rendering space
		float32_t3 rgbPre = mul(oces, AP0_2_AP1_MAT);

		// Apply the tonescale independently in rendering-space RGB
		float32_t3 rgbPost = SegmentedSplineC9Fwd(rgbPre, p.tonescale);

		float32_t3 linearCV;
		if (p.sdrCinemaToVideo) {
			//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/odt/sRGB/ODT.Academy.sRGB_100nits_dim.ctl#L89-L99

			// Scale luminance to linear code value
			linearCV = YToLinCV(rgbPost, CINEMA_WHITE, CINEMA_BLACK);

			// Apply gamma adjustment to compensate for dim surround
			linearCV = DarkSurroundToDimSurround(linearCV);

			// Apply desaturation to compensate for luminance difference
			linearCV = mul(linearCV, ODT_SAT_MAT);

		} else {
			//! @reference https://github.com/aces-aswf/aces-core/blob/ed0c686a2067ee1daf60178fecc88633a0af26dc/transforms/ctl/odt/hdr_st2084/ODT.Academy.Rec2020_ST2084_1000nits.ctl#L75-L76
			//! @reference https://github.com/aces-aswf/aces-core/blob/ed0c686a2067ee1daf60178fecc88633a0af26dc/transforms/ctl/odt/hdr_st2084/ODT.Academy.P3D60_ST2084_2000nits.ctl#L74-L75

			// Subtract small offset to allow for a code value of 0
			linearCV = YToLinCV(rgbPost - ODT_HDR_BLACK_OFFSET, p.Y_MAX, p.Y_MIN);
			// note: cd/m^2 -> 表示の Y_MIN .. Y_MAX に対する相対値 (outputTransform() と同じ扱い)
		}

		// Convert to display primary encoding
		// Rendering space RGB to XYZ
		float32_t3 XYZ = mul(linearCV, AP1_2_XYZ_MAT);

		// Apply CAT from ACES white point to assumed observer adapted white point
		if (p.applyCAT) {
			//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/odt/sRGB/ODT.Academy.sRGB_100nits_dim.ctl#L105-L106

			XYZ = mul(XYZ, D60_2_D65_CAT); //!< D60 -> D65
		}

		// CIE XYZ to display primaries
		float32_t3 rgb = mul(XYZ, p.XYZ_to_display);

		// Handle out-of-gamut values
		if (p.sdrCinemaToVideo) {
			//! @reference https://github.com/aces-aswf/aces-core/blob/fc189808208e28019a8c5a27cdb2d50312939232/transforms/ctl/odt/sRGB/ODT.Academy.sRGB_100nits_dim.ctl#L111-L113

			rgb = saturate(rgb); //!< SDR ODT は 0-1 にクリップ

		} else {
			//! @reference https://github.com/aces-aswf/aces-core/blob/ed0c686a2067ee1daf60178fecc88633a0af26dc/transforms/ctl/odt/hdr_st2084/ODT.Academy.Rec2020_ST2084_1000nits.ctl#L88-L90
			//! @reference https://github.com/aces-aswf/aces-core/blob/ed0c686a2067ee1daf60178fecc88633a0af26dc/transforms/ctl/odt/hdr_st2084/ODT.Academy.P3D60_ST2084_2000nits.ctl#L84-L86

			rgb = max(rgb, float32_t3(0.0, 0.0, 0.0)); //!< HDR ODT は 0 以上にクリップ
		}

		return rgb;
	}

}

#endif
