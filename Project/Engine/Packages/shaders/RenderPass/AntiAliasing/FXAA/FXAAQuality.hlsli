#pragma once

// # reference
// [Fxaa_3.11.hlsli](https://gist.github.com/jwoolston/c96c22473110c9924d831cebb5f2cdb4)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "FXAADefine.hlsli"
#include "FXAACommon.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// FXAA namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace FXAA {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	Bool EdgeSearchStep(
		Tex t,
		Float2 offNP,
		Float lumaNN,
		Float gradientScaled,
		Float step_size,
		inout Float2 posN,
		inout Float2 posP,
		inout Float lumaEndN,
		inout Float lumaEndP,
		inout Bool doneN,
		inout Bool doneP) {

		if (!doneN) lumaEndN = Luma(TexTop(t, posN).rgb);
		if (!doneP) lumaEndP = Luma(TexTop(t, posP).rgb);
		if (!doneN) lumaEndN = lumaEndN - lumaNN * 0.5f;
		if (!doneP) lumaEndP = lumaEndP - lumaNN * 0.5f;

		doneN = abs(lumaEndN) >= gradientScaled;
		doneP = abs(lumaEndP) >= gradientScaled;

		if (!doneN) posN -= offNP * step_size;
		if (!doneP) posP += offNP * step_size;

		return (!doneN) || (!doneP);
	}

	//! @brief FXAA (PC Quality) を適用する.
	//! @param[in] texcoord ピクセル中心
	//! @param[in] t 入力テクスチャ
	//! @param[in] rcp_frame 1.0 / 解像度
	//! @param[in] subpix サブピクセルAAの強さ. 1.00(ソフト) / 0.75(既定) / 0.50(シャープ) / 0.25(ほぼ無効) / 0.00(無効)
	//! @param[in] edge_threshold 処理に必要な最小のコントラスト. 0.333(少なすぎ) / 0.250(低品質) / 0.166(既定) / 0.125(高品質) / 0.063(過剰)
	//! @param[in] edge_threshold_min 暗部を処理から除外する閾値. 0.0833(既定) / 0.0625(高品質) / 0.0312(可視限界)
	Float4 FXAAQuality(
		Float2 texcoord,
		Tex t,
		Float2 rcp_frame,
		Float subpix,
		Float edge_threshold,
		Float edge_threshold_min) {

		Float2 posM = texcoord;

		//* 中央と上下左右の輝度 *//

		Float4 rgbyM = TexTop(t, posM);
		Float lumaM  = Luma(rgbyM.rgb);
		Float lumaS  = Luma(TexTopOff(t, posM, Int2( 0,  1)).rgb);
		Float lumaE  = Luma(TexTopOff(t, posM, Int2( 1,  0)).rgb);
		Float lumaN  = Luma(TexTopOff(t, posM, Int2( 0, -1)).rgb);
		Float lumaW  = Luma(TexTopOff(t, posM, Int2(-1,  0)).rgb);

		Float maxSM    = max(lumaS, lumaM);
		Float minSM    = min(lumaS, lumaM);
		Float maxESM   = max(lumaE, maxSM);
		Float minESM   = min(lumaE, minSM);
		Float maxWN    = max(lumaN, lumaW);
		Float minWN    = min(lumaN, lumaW);
		Float rangeMax = max(maxWN, maxESM);
		Float rangeMin = min(minWN, minESM);
		Float range    = rangeMax - rangeMin;

		//* 早期終了 *//

#if (FXAA_EARLY_EXIT == 1)
		Float rangeMaxScaled  = rangeMax * edge_threshold;
		Float rangeMaxClamped = max(edge_threshold_min, rangeMaxScaled);

		if (range < rangeMaxClamped) {
#if (FXAA_DISCARD == 1)
			discard;
#endif
			return rgbyM;
		}
#endif

		//* 斜め4方向の輝度 *//

		Float lumaNW = Luma(TexTopOff(t, posM, Int2(-1, -1)).rgb);
		Float lumaSE = Luma(TexTopOff(t, posM, Int2( 1,  1)).rgb);
		Float lumaNE = Luma(TexTopOff(t, posM, Int2( 1, -1)).rgb);
		Float lumaSW = Luma(TexTopOff(t, posM, Int2(-1,  1)).rgb);

		//* エッジ方向の判定 *//

		Float lumaNS         = lumaN + lumaS;
		Float lumaWE         = lumaW + lumaE;
		Float subpixRcpRange = 1.0f / range;
		Float subpixNSWE     = lumaNS + lumaWE;
		Float edgeHorz1      = (-2.0f * lumaM) + lumaNS;
		Float edgeVert1      = (-2.0f * lumaM) + lumaWE;

		Float lumaNESE  = lumaNE + lumaSE;
		Float lumaNWNE  = lumaNW + lumaNE;
		Float edgeHorz2 = (-2.0f * lumaE) + lumaNESE;
		Float edgeVert2 = (-2.0f * lumaN) + lumaNWNE;

		Float lumaNWSW  = lumaNW + lumaSW;
		Float lumaSWSE  = lumaSW + lumaSE;
		Float edgeHorz4 = (abs(edgeHorz1) * 2.0f) + abs(edgeHorz2);
		Float edgeVert4 = (abs(edgeVert1) * 2.0f) + abs(edgeVert2);
		Float edgeHorz3 = (-2.0f * lumaW) + lumaNWSW;
		Float edgeVert3 = (-2.0f * lumaS) + lumaSWSE;
		Float edgeHorz  = abs(edgeHorz3) + edgeHorz4;
		Float edgeVert  = abs(edgeVert3) + edgeVert4;

		Float subpixNWSWNESE = lumaNWSW + lumaNESE;
		Float lengthSign     = rcp_frame.x;
		Bool horzSpan        = edgeHorz >= edgeVert;
		Float subpixA        = subpixNSWE * 2.0f + subpixNWSWNESE;

		if (!horzSpan) lumaN = lumaW;
		if (!horzSpan) lumaS = lumaE;
		if (horzSpan) lengthSign = rcp_frame.y;
		Float subpixB = (subpixA * (1.0f / 12.0f)) - lumaM;

		Float gradientN = lumaN - lumaM;
		Float gradientS = lumaS - lumaM;
		Float lumaNN    = lumaN + lumaM;
		Float lumaSS    = lumaS + lumaM;
		Bool pairN      = abs(gradientN) >= abs(gradientS);
		Float gradient  = max(abs(gradientN), abs(gradientS));
		if (pairN) lengthSign = -lengthSign;
		Float subpixC = saturate(abs(subpixB) * subpixRcpRange);

		//* エッジ端の探索 *//

		Float2 posB = posM;
		Float2 offNP;
		offNP.x = (!horzSpan) ? 0.0f : rcp_frame.x;
		offNP.y = ( horzSpan) ? 0.0f : rcp_frame.y;
		if (!horzSpan) posB.x += lengthSign * 0.5f;
		if ( horzSpan) posB.y += lengthSign * 0.5f;

		Float2 posN    = posB - offNP * FXAA_QUALITY__P0;
		Float2 posP    = posB + offNP * FXAA_QUALITY__P0;
		Float subpixD  = ((-2.0f) * subpixC) + 3.0f;
		Float lumaEndN = Luma(TexTop(t, posN).rgb);
		Float subpixE  = subpixC * subpixC;
		Float lumaEndP = Luma(TexTop(t, posP).rgb);

		if (!pairN) lumaNN = lumaSS;
		Float gradientScaled = gradient * 0.25f;
		Float lumaMM         = lumaM - lumaNN * 0.5f;
		Float subpixF        = subpixD * subpixE;
		Bool lumaMLTZero     = lumaMM < 0.0f;

		lumaEndN -= lumaNN * 0.5f;
		lumaEndP -= lumaNN * 0.5f;
		Bool doneN = abs(lumaEndN) >= gradientScaled;
		Bool doneP = abs(lumaEndP) >= gradientScaled;
		if (!doneN) posN -= offNP * FXAA_QUALITY__P1;
		if (!doneP) posP += offNP * FXAA_QUALITY__P1;
		Bool doneNP = (!doneN) || (!doneP); //!< 探索が終わっていない側があるか

#define FXAA_EDGE_SEARCH_STEP(step_size) \
		if (doneNP) { doneNP = EdgeSearchStep(t, offNP, lumaNN, gradientScaled, step_size, posN, posP, lumaEndN, lumaEndP, doneN, doneP); }

		FXAA_EDGE_SEARCH_STEP(FXAA_QUALITY__P2)
#if (FXAA_QUALITY__PS > 3)
		FXAA_EDGE_SEARCH_STEP(FXAA_QUALITY__P3)
#endif
#if (FXAA_QUALITY__PS > 4)
		FXAA_EDGE_SEARCH_STEP(FXAA_QUALITY__P4)
#endif
#if (FXAA_QUALITY__PS > 5)
		FXAA_EDGE_SEARCH_STEP(FXAA_QUALITY__P5)
#endif
#if (FXAA_QUALITY__PS > 6)
		FXAA_EDGE_SEARCH_STEP(FXAA_QUALITY__P6)
#endif
#if (FXAA_QUALITY__PS > 7)
		FXAA_EDGE_SEARCH_STEP(FXAA_QUALITY__P7)
#endif
#if (FXAA_QUALITY__PS > 8)
		FXAA_EDGE_SEARCH_STEP(FXAA_QUALITY__P8)
#endif
#if (FXAA_QUALITY__PS > 9)
		FXAA_EDGE_SEARCH_STEP(FXAA_QUALITY__P9)
#endif
#if (FXAA_QUALITY__PS > 10)
		FXAA_EDGE_SEARCH_STEP(FXAA_QUALITY__P10)
#endif
#if (FXAA_QUALITY__PS > 11)
		FXAA_EDGE_SEARCH_STEP(FXAA_QUALITY__P11)
#endif

#undef FXAA_EDGE_SEARCH_STEP

		//* サンプル位置のオフセット計算 *//

		Float dstN = posM.x - posN.x;
		Float dstP = posP.x - posM.x;
		if (!horzSpan) dstN = posM.y - posN.y;
		if (!horzSpan) dstP = posP.y - posM.y;

		Bool goodSpanN      = (lumaEndN < 0.0f) != lumaMLTZero;
		Float spanLength    = (dstP + dstN);
		Bool goodSpanP      = (lumaEndP < 0.0f) != lumaMLTZero;
		Float spanLengthRcp = 1.0f / spanLength;

		Bool directionN   = dstN < dstP;
		Float dstMin      = min(dstN, dstP);
		Bool goodSpan     = directionN ? goodSpanN : goodSpanP;
		Float subpixG     = subpixF * subpixF;
		Float pixelOffset = (dstMin * (-spanLengthRcp)) + 0.5f;
		Float subpixH     = subpixG * subpix;

		Float pixelOffsetGood   = goodSpan ? pixelOffset : 0.0f;
		Float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
		if (!horzSpan) posM.x += pixelOffsetSubpix * lengthSign;
		if ( horzSpan) posM.y += pixelOffsetSubpix * lengthSign;

		return TexTop(t, posM);
	}


};
