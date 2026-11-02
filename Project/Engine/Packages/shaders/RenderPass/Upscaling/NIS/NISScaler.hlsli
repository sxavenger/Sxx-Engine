#pragma once

// # reference
// [NIS_Scaler.h](https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "NISDefine.hlsli"
#include "NISCommon.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// NIS namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace NIS {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	NVF getY(NVF3 rgb) {
#if NIS_HDR_MODE == NIS_HDR_MODE_PQ
		return NVF(0.262f) * rgb.x + NVF(0.678f) * rgb.y + NVF(0.0593f) * rgb.z;
#elif NIS_HDR_MODE == NIS_HDR_MODE_LINEAR
		return sqrt(NVF(0.2126f) * rgb.x + NVF(0.7152f) * rgb.y + NVF(0.0722f) * rgb.z) * kHDRCompressionFactor;
#else
		return NVF(0.2126f) * rgb.x + NVF(0.7152f) * rgb.y + NVF(0.0722f) * rgb.z;
#endif
	}

	NVF getYLinear(NVF3 rgb) {
		return NVF(0.2126f) * rgb.x + NVF(0.7152f) * rgb.y + NVF(0.0722f) * rgb.z;
	}

	NVF3 YUVtoRGB(NVF3 yuv) {
		float y = yuv.x - 16.0f / 255.0f;
		float u = yuv.y - 128.0f / 255.0f;
		float v = yuv.z - 128.0f / 255.0f;
		NVF3 rgb;
		rgb.x = saturate(1.164f * y + 1.596f * v);
		rgb.y = saturate(1.164f * y - 0.392f * u - 0.813f * v);
		rgb.z = saturate(1.164f * y + 2.017f * u);
		return rgb;
	}

#if NIS_SCALER
	NVF4 GetEdgeMap(Config config, NVF p[4][4], NVI i, NVI j)
#else
	NVF4 GetEdgeMap(Config config, NVF p[5][5], NVI i, NVI j)
#endif
	{

		const NVF g_0 = abs(p[0 + i][0 + j] + p[0 + i][1 + j] + p[0 + i][2 + j] - p[2 + i][0 + j] - p[2 + i][1 + j] - p[2 + i][2 + j]);
		const NVF g_45 = abs(p[1 + i][0 + j] + p[0 + i][0 + j] + p[0 + i][1 + j] - p[2 + i][1 + j] - p[2 + i][2 + j] - p[1 + i][2 + j]);
		const NVF g_90 = abs(p[0 + i][0 + j] + p[1 + i][0 + j] + p[2 + i][0 + j] - p[0 + i][2 + j] - p[1 + i][2 + j] - p[2 + i][2 + j]);
		const NVF g_135 = abs(p[1 + i][0 + j] + p[2 + i][0 + j] + p[2 + i][1 + j] - p[0 + i][1 + j] - p[0 + i][2 + j] - p[1 + i][2 + j]);

		const NVF g_0_90_max = max(g_0, g_90);
		const NVF g_0_90_min = min(g_0, g_90);
		const NVF g_45_135_max = max(g_45, g_135);
		const NVF g_45_135_min = min(g_45, g_135);

		NVF e_0_90 = 0;
		NVF e_45_135 = 0;

		if (g_0_90_max + g_45_135_max == 0) {
			return NVF4(0, 0, 0, 0);
		}

		e_0_90 = min(g_0_90_max / (g_0_90_max + g_45_135_max), 1.0f);
		e_45_135 = 1.0f - e_0_90;

		NVB c_0_90 = (g_0_90_max > (g_0_90_min * config.detect_ratio)) && (g_0_90_max > config.detect_thres) && (g_0_90_max > g_45_135_min);
		NVB c_45_135 = (g_45_135_max > (g_45_135_min * config.detect_ratio)) && (g_45_135_max > config.detect_thres) && (g_45_135_max > g_0_90_min);
		NVB c_g_0_90 = g_0_90_max == g_0;
		NVB c_g_45_135 = g_45_135_max == g_45;

		NVF f_e_0_90 = (c_0_90 && c_45_135) ? e_0_90 : 1.0f;
		NVF f_e_45_135 = (c_0_90 && c_45_135) ? e_45_135 : 1.0f;

		NVF weight_0 = (c_0_90 && c_g_0_90) ? f_e_0_90 : 0.0f;
		NVF weight_90 = (c_0_90 && !c_g_0_90) ? f_e_0_90 : 0.0f;
		NVF weight_45 = (c_45_135 && c_g_45_135) ? f_e_45_135 : 0.0f;
		NVF weight_135 = (c_45_135 && !c_g_45_135) ? f_e_45_135 : 0.0f;

		return NVF4(weight_0, weight_90, weight_45, weight_135);
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// [NVScaler] methods
	////////////////////////////////////////////////////////////////////////////////////////////
#if NIS_SCALER

	//!< groupshared variables
	groupshared NVF  shPixelsY[NIS_TILE_SIZE];
	groupshared NVH  shCoefScaler[NIS_PHASE_COUNT][NIS_FILTER_SIZE];
	groupshared NVH  shCoefUSM[NIS_PHASE_COUNT][NIS_FILTER_SIZE];
	groupshared NVH4 shEdgeMap[NIS_EDGE_MAP_SIZE];

	// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L356-L388
	void LoadFilterBanksSh(Bindings bindings, NVI i0) {
		// Load up filter banks to shared memory
		// The work is spread over (NIS_PHASE_COUNT * 2) threads
		NVI i = i0;
#if( NIS_PHASE_COUNT * 2 > NIS_THREAD_GROUP_SIZE )
		for (; i < NIS_PHASE_COUNT * 2; i += NIS_THREAD_GROUP_SIZE)
#else
		if (i < NIS_PHASE_COUNT * 2)
#endif
		{
			NVI phase = i >> 1;
			NVI vIdx = i & 1;

			NVH4 v = NVH4(bindings.coef_scaler[NVI2(vIdx, phase)]);
			NVI filterOffset = vIdx * 4;
			shCoefScaler[phase][filterOffset + 0] = v.x;
			shCoefScaler[phase][filterOffset + 1] = v.y;
			if (vIdx == 0) {
				shCoefScaler[phase][2] = v.z;
				shCoefScaler[phase][3] = v.w;
			}

			v = NVH4(bindings.coef_usm[NVI2(vIdx, phase)]);
			shCoefUSM[phase][filterOffset + 0] = v.x;
			shCoefUSM[phase][filterOffset + 1] = v.y;
			if (vIdx == 0)
			{
				shCoefUSM[phase][2] = v.z;
				shCoefUSM[phase][3] = v.w;
			}
		}
	}

	// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L391-L406
	NVF CalcLTI(Config config, NVF p0, NVF p1, NVF p2, NVF p3, NVF p4, NVF p5, NVI phase_index) {
		const NVB selector = (phase_index <= NIS_PHASE_COUNT / 2);
		NVF sel = selector ? p0 : p3;
		const NVF a_min = min(min(p1, p2), sel);
		const NVF a_max = max(max(p1, p2), sel);
		sel = selector ? p2 : p5;
		const NVF b_min = min(min(p3, p4), sel);
		const NVF b_max = max(max(p3, p4), sel);

		const NVF a_cont = a_max - a_min;
		const NVF b_cont = b_max - b_min;

		const NVF cont_ratio = max(a_cont, b_cont) / (min(a_cont, b_cont) + config.eps);
		return (1.0f - saturate((cont_ratio - config.min_contrast_ratio) * config.ratio_norm)) * config.contrast_boost;
	}

	// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L408-L413
	NVF4 GetInterpEdgeMap(const NVF4 edge[2][2], NVF phase_frac_x, NVF phase_frac_y) {
		NVF4 h0 = lerp(edge[0][0], edge[0][1], phase_frac_x);
		NVF4 h1 = lerp(edge[1][0], edge[1][1], phase_frac_x);
		return lerp(h0, h1, phase_frac_y);
	}

	// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L415-L450
	NVF EvalPoly6(Config config, const NVF pxl[6], NVI phase_int) {
		NVF y = 0.f;
		[unroll]
		for (NVI i = 0; i < 6; ++i) {
			y += shCoefScaler[phase_int][i] * pxl[i];
		}

		NVF y_usm = 0.f;
		[unroll]
		for (NVI i = 0; i < 6; ++i) {
			y_usm += shCoefUSM[phase_int][i] * pxl[i];
		}

		// let's compute a piece-wise ramp based on luma
		const NVF y_scale = 1.0f - saturate((y * (1.0f / NIS_SCALE_FLOAT) - config.sharp_start_y) * config.sharp_scale_y);

		// scale the ramp to sharpen as a function of luma
		const NVF y_sharpness = y_scale * config.sharp_strength_scale + config.sharp_strength_min;

		y_usm *= y_sharpness;

		// scale the ramp to limit USM as a function of luma
		const NVF y_sharpness_limit = (y_scale * config.sharp_limit_scale + config.sharp_limit_min) * y;

		y_usm = min(y_sharpness_limit, max(-y_sharpness_limit, y_usm));
		// reduce ringing
		y_usm *= CalcLTI(config, pxl[0], pxl[1], pxl[2], pxl[3], pxl[4], pxl[5], phase_int);

		return y + y_usm;
	}

	// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L452-L469
	NVF FilterNormal(const NVF p[6][6], NVI phase_x_frac_int, NVI phase_y_frac_int) {
		NVF h_acc = 0.0f;
		[unroll]
		for (NVI j = 0; j < 6; ++j) {
			NVF v_acc = 0.0f;
			[unroll]
			for (NVI i = 0; i < 6; ++i) {
				v_acc += p[i][j] * shCoefScaler[phase_y_frac_int][i];
			}
			h_acc += v_acc * shCoefScaler[phase_x_frac_int][j];
		}

		// let's return the sum unpacked -> we can accumulate it later
		return h_acc;
	}

	// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L471-L588
	NVF AddDirFilters(Config config, NVF p[6][6], NVF phase_x_frac, NVF phase_y_frac, NVI phase_x_frac_int, NVI phase_y_frac_int, NVF4 w) {
		NVF f = 0;

		if (w.x > 0.0f) {
			// 0 deg filter
			NVF interp0Deg[6];
			[unroll]
			for (NVI i = 0; i < 6; ++i) {
				interp0Deg[i] = lerp(p[i][2], p[i][3], phase_x_frac);
			}

			f += EvalPoly6(config, interp0Deg, phase_y_frac_int) * w.x;
		}

		if (w.y > 0.0f) {
			// 90 deg filter
			NVF interp90Deg[6];
			[unroll]
			for (NVI i = 0; i < 6; ++i) {
				interp90Deg[i] = lerp(p[2][i], p[3][i], phase_y_frac);
			}

			f += EvalPoly6(config, interp90Deg, phase_x_frac_int) * w.y;
		}

		if (w.z > 0.0f) {
			// 45 deg filter
			NVF pphase_b45 = 0.5f + 0.5f * (phase_x_frac - phase_y_frac);

			NVF temp_interp45Deg[7];
			temp_interp45Deg[1] = lerp(p[2][1], p[1][2], pphase_b45);
			temp_interp45Deg[3] = lerp(p[3][2], p[2][3], pphase_b45);
			temp_interp45Deg[5] = lerp(p[4][3], p[3][4], pphase_b45);
			{
				pphase_b45 = pphase_b45 - 0.5f;
				NVF a = (pphase_b45 >= 0.f) ? p[0][2] : p[2][0];
				NVF b = (pphase_b45 >= 0.f) ? p[1][3] : p[3][1];
				NVF c = (pphase_b45 >= 0.f) ? p[2][4] : p[4][2];
				NVF d = (pphase_b45 >= 0.f) ? p[3][5] : p[5][3];
				temp_interp45Deg[0] = lerp(p[1][1], a, abs(pphase_b45));
				temp_interp45Deg[2] = lerp(p[2][2], b, abs(pphase_b45));
				temp_interp45Deg[4] = lerp(p[3][3], c, abs(pphase_b45));
				temp_interp45Deg[6] = lerp(p[4][4], d, abs(pphase_b45));
			}

			NVF interp45Deg[6];
			NVF pphase_p45 = phase_x_frac + phase_y_frac;
			if (pphase_p45 >= 1) {
				[unroll]
				for (NVI i = 0; i < 6; i++) {
					interp45Deg[i] = temp_interp45Deg[i + 1];
				}
				pphase_p45 = pphase_p45 - 1;

			} else {
				[unroll]
				for (NVI i = 0; i < 6; i++) {
					interp45Deg[i] = temp_interp45Deg[i];
				}
			}

			f += EvalPoly6(config, interp45Deg, NVI(pphase_p45 * NIS_PHASE_COUNT)) * w.z;
		}

		if (w.w > 0.0f) {
			// 135 deg filter
			NVF pphase_b135 = 0.5f * (phase_x_frac + phase_y_frac);

			NVF temp_interp135Deg[7];
			temp_interp135Deg[1] = lerp(p[3][1], p[4][2], pphase_b135);
			temp_interp135Deg[3] = lerp(p[2][2], p[3][3], pphase_b135);
			temp_interp135Deg[5] = lerp(p[1][3], p[2][4], pphase_b135);
			{
				pphase_b135 = pphase_b135 - 0.5f;
				NVF a = (pphase_b135 >= 0.f) ? p[5][2] : p[3][0];
				NVF b = (pphase_b135 >= 0.f) ? p[4][3] : p[2][1];
				NVF c = (pphase_b135 >= 0.f) ? p[3][4] : p[1][2];
				NVF d = (pphase_b135 >= 0.f) ? p[2][5] : p[0][3];
				temp_interp135Deg[0] = lerp(p[4][1], a, abs(pphase_b135));
				temp_interp135Deg[2] = lerp(p[3][2], b, abs(pphase_b135));
				temp_interp135Deg[4] = lerp(p[2][3], c, abs(pphase_b135));
				temp_interp135Deg[6] = lerp(p[1][4], d, abs(pphase_b135));
			}

			NVF interp135Deg[6];
			NVF pphase_p135 = 1 + (phase_x_frac - phase_y_frac);
			if (pphase_p135 >= 1) {
				[unroll]
				for (NVI i = 0; i < 6; ++i) {
					interp135Deg[i] = temp_interp135Deg[i + 1];
				}
				pphase_p135 = pphase_p135 - 1;

			} else {
				[unroll]
				for (NVI i = 0; i < 6; ++i) {
					interp135Deg[i] = temp_interp135Deg[i];
				}
			}

			f += EvalPoly6(config, interp135Deg, NVI(pphase_p135 * NIS_PHASE_COUNT)) * w.w;
		}

		return f;
	}

	// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L591-L814
	void NVScaler(Config config, Bindings bindings, NVU2 blockIdx, NVU threadIdx) {
		// Figure out the range of pixels from input image that would be needed to be loaded for this thread-block
		NVI dstBlockX = NVI(NIS_BLOCK_WIDTH * blockIdx.x);
		NVI dstBlockY = NVI(NIS_BLOCK_HEIGHT * blockIdx.y);

		const NVI srcBlockStartX = NVI(floor((dstBlockX + 0.5f) * config.scale.x - 0.5f));
		const NVI srcBlockStartY = NVI(floor((dstBlockY + 0.5f) * config.scale.y - 0.5f));
		const NVI srcBlockEndX   = NVI(ceil((dstBlockX + NIS_BLOCK_WIDTH + 0.5f) * config.scale.x - 0.5f));
		const NVI srcBlockEndY   = NVI(ceil((dstBlockY + NIS_BLOCK_HEIGHT + 0.5f) * config.scale.y - 0.5f));

		NVI numTilePixelsX = srcBlockEndX - srcBlockStartX + NIS_SUPPORT_SIZE - 1;
		NVI numTilePixelsY = srcBlockEndY - srcBlockStartY + NIS_SUPPORT_SIZE - 1;

		// round-up load region to even size since we're loading in 2x2 batches
		numTilePixelsX += numTilePixelsX & 0x1;
		numTilePixelsY += numTilePixelsY & 0x1;
		const NVI numTilePixels = numTilePixelsX * numTilePixelsY;

		// calculate the equivalent values for the edge map
		const NVI numEdgeMapPixelsX = numTilePixelsX - NIS_SUPPORT_SIZE + 2;
		const NVI numEdgeMapPixelsY = numTilePixelsY - NIS_SUPPORT_SIZE + 2;
		const NVI numEdgeMapPixels  = numEdgeMapPixelsX * numEdgeMapPixelsY;

		// fill in input luma tile (shPixelsY) in batches of 2x2 pixels
		// we use texture gather to get extra support necessary
		// to compute 2x2 edge map outputs too
		for (NVU i = threadIdx * 2; i < NVU(numTilePixels) >> 1; i += NIS_THREAD_GROUP_SIZE * 2) {
			NVU py = (i / numTilePixelsX) * 2;
			NVU px = i % numTilePixelsX;

			// 0.5 to be in the center of texel
			// - (NIS_SUPPORT_SIZE - 1) / 2 to shift by the kernel support size
			NVF kShift = 0.5f - (NIS_SUPPORT_SIZE - 1) / 2;
#if NIS_VIEWPORT_SUPPORT
			const NVF tx = (NVF(srcBlockStartX) + NVF(px) + NVF(config.input_viewport_origin.x) + kShift) * config.src_norm.x;
			const NVF ty = (NVF(srcBlockStartY) + NVF(py) + NVF(config.input_viewport_origin.y) + kShift) * config.src_norm.y;
#else
			const NVF tx = (NVF(srcBlockStartX) + NVF(px) + kShift) * config.src_norm.x;
			const NVF ty = (NVF(srcBlockStartY) + NVF(py) + kShift) * config.src_norm.y;
#endif

			NVF p[2][2];
#if NIS_TEXTURE_GATHER
			{
				const NVF4 sr = bindings.in_texture.GatherRed(bindings.sampler_linear_clamp, NVF2(tx, ty));
				const NVF4 sg = bindings.in_texture.GatherGreen(bindings.sampler_linear_clamp, NVF2(tx, ty));
				const NVF4 sb = bindings.in_texture.GatherBlue(bindings.sampler_linear_clamp, NVF2(tx, ty));

				p[0][0] = getY(NVF3(sr.w, sg.w, sb.w));
				p[0][1] = getY(NVF3(sr.z, sg.z, sb.z));
				p[1][0] = getY(NVF3(sr.x, sg.x, sb.x));
				p[1][1] = getY(NVF3(sr.y, sg.y, sb.y));
			}
#else
			[unroll]
			for (NVI j = 0; j < 2; j++) {
				[unroll]
				for (NVI k = 0; k < 2; k++) {
					const NVF2 uv = NVF2(tx + k * config.src_norm.x, ty + j * config.src_norm.y);
#if NIS_NV12_SUPPORT
					p[j][k] = bindings.in_texture_y.SampleLevel(bindings.sampler_linear_clamp, uv, 0);
#else
					const NVF4 color = bindings.in_texture.SampleLevel(bindings.sampler_linear_clamp, uv, 0);
					p[j][k] = getY(color.xyz);
#endif
				}
			}
#endif

			const NVU idx = py * NIS_TILE_PITCH + px;
			shPixelsY[idx]                      = NVH(p[0][0]);
			shPixelsY[idx + 1]                  = NVH(p[0][1]);
			shPixelsY[idx + NIS_TILE_PITCH]     = NVH(p[1][0]);
			shPixelsY[idx + NIS_TILE_PITCH + 1] = NVH(p[1][1]);
		}

		GroupMemoryBarrierWithGroupSync();

		// fill in the edge map of 2x2 pixels
		for (NVU i = threadIdx * 2; i < NVU(numEdgeMapPixels) >> 1; i += NIS_THREAD_GROUP_SIZE * 2) {
			NVU py = (i / numEdgeMapPixelsX) * 2;
			NVU px = i % numEdgeMapPixelsX;

			const NVU edgeMapIdx = py * NIS_EDGE_MAP_PITCH + px;

			NVU tileCornerIdx = (py + 1) * NIS_TILE_PITCH + px + 1;
			NVF p[4][4];
			[unroll]
			for (NVI j = 0; j < 4; j++) {
				[unroll]
				for (NVI k = 0; k < 4; k++) {
					p[j][k] = shPixelsY[tileCornerIdx + j * NIS_TILE_PITCH + k];
				}
			}

			shEdgeMap[edgeMapIdx]                          = NVH4(GetEdgeMap(config, p, 0, 0));
			shEdgeMap[edgeMapIdx + 1]                      = NVH4(GetEdgeMap(config, p, 0, 1));
			shEdgeMap[edgeMapIdx + NIS_EDGE_MAP_PITCH]     = NVH4(GetEdgeMap(config, p, 1, 0));
			shEdgeMap[edgeMapIdx + NIS_EDGE_MAP_PITCH + 1] = NVH4(GetEdgeMap(config, p, 1, 1));
		}

		LoadFilterBanksSh(bindings, NVI(threadIdx));
		GroupMemoryBarrierWithGroupSync();

		// output coord within a tile
		const NVI2 pos = NVI2(NVU(threadIdx) % NVU(NIS_BLOCK_WIDTH), NVU(threadIdx) / NVU(NIS_BLOCK_WIDTH));
		// x coord inside the output image
		const NVI dstX = dstBlockX + pos.x;
		// x coord inside the input image
		const NVF srcX = (0.5f + dstX) * config.scale.x - 0.5f;
		// nearest integer part
		const NVI px = NVI(floor(srcX) - srcBlockStartX);
		// fractional part
		const NVF fx = srcX - floor(srcX);
		// discretized phase
		const NVI fx_int = NVI(fx * NIS_PHASE_COUNT);

#if NIS_VIEWPORT_SUPPORT
		if (NVU(srcX) > config.input_viewport_size.x || NVU(dstX) > config.output_viewport_size.x) {
			return;
		}
#endif

		for (NVI k = 0; k < NIS_BLOCK_WIDTH * NIS_BLOCK_HEIGHT / NIS_THREAD_GROUP_SIZE; ++k) {
			// y coord inside the output image
			const NVI dstY = dstBlockY + pos.y + k * (NIS_THREAD_GROUP_SIZE / NIS_BLOCK_WIDTH);
			// y coord inside the input image
			const NVF srcY = (0.5f + dstY) * config.scale.y - 0.5f;

#if NIS_VIEWPORT_SUPPORT
			if (NVU(srcY) > config.input_viewport_size.y || NVU(dstY) > config.output_viewport_size.y) {
				continue;
			}
#endif

			// nearest integer part
			const NVI py = NVI(floor(srcY) - srcBlockStartY);
			// fractional part
			const NVF fy = srcY - floor(srcY);
			// discretized phase
			const NVI fy_int = NVI(fy * NIS_PHASE_COUNT);

			// generate weights for directional filters
			const NVI startEdgeMapIdx = py * NIS_EDGE_MAP_PITCH + px;
			NVF4 edge[2][2];
			[unroll]
			for (NVI i = 0; i < 2; i++) {
				[unroll]
				for (NVI j = 0; j < 2; j++) {
					// need to shift edge map sampling since it's a 2x2 centered inside 6x6 grid
					edge[i][j] = shEdgeMap[startEdgeMapIdx + (i * NIS_EDGE_MAP_PITCH) + j];
				}
			}
			const NVF4 w = GetInterpEdgeMap(edge, fx, fy) * NIS_SCALE_INT;

			// load 6x6 support to regs
			const NVI startTileIdx = py * NIS_TILE_PITCH + px;
			NVF p[6][6];
			[unroll]
			for (NVI i = 0; i < 6; ++i) {
				[unroll]
				for (NVI j = 0; j < 6; ++j) {
					p[i][j] = shPixelsY[startTileIdx + i * NIS_TILE_PITCH + j];
				}
			}

			// weigth for luma
			const NVF baseWeight = NIS_SCALE_FLOAT - w.x - w.y - w.z - w.w;

			// final luma is a weighted product of directional & normal filters
			NVF opY = 0;

			// get traditional scaler filter output
			opY += FilterNormal(p, fx_int, fy_int) * baseWeight;

			// get directional filter bank output
			opY += AddDirFilters(config, p, fx, fy, fx_int, fy_int, w);

#if NIS_VIEWPORT_SUPPORT
			NVF2 coord    = NVF2((srcX + config.input_viewport_origin.x + 0.5f) * config.src_norm.x, (srcY + config.input_viewport_origin.y + 0.5f) * config.src_norm.y);
			NVU2 dstCoord = NVU2(dstX, dstY) + config.output_viewport_origin;
#else
			NVF2 coord    = NVF2((srcX + 0.5f) * config.src_norm.x, (srcY + 0.5f) * config.src_norm.y);
			NVU2 dstCoord = NVU2(dstX, dstY);
#endif

			// do bilinear tap for chroma upscaling
#if NIS_NV12_SUPPORT
			NVF  y  = bindings.in_texture_y.SampleLevel(bindings.sampler_linear_clamp, coord, 0);
			NVF2 uv = bindings.in_texture_uv.SampleLevel(bindings.sampler_linear_clamp, coord, 0);
			NVF4 op = NVF4(YUVtoRGB(NVF3(y, uv)), 1.0f);
#else
			NVF4 op = bindings.in_texture.SampleLevel(bindings.sampler_linear_clamp, coord, 0);
			NVF  y  = getY(op.xyz);
#endif

#if NIS_HDR_MODE == NIS_HDR_MODE_LINEAR
			const NVF kEps  = 1e-4f;
			const NVF kNorm = 1.0f / (NIS_SCALE_FLOAT * kHDRCompressionFactor);
			const NVF opYN  = max(opY, 0.0f) * kNorm;
			const NVF corr  = (opYN * opYN + kEps) / (max(getYLinear(op.xyz), 0.0f) + kEps);
			op.xyz *= corr;
#else
			const NVF corr = opY * (1.0f / NIS_SCALE_FLOAT) - y;
			op.xyz += corr;
#endif

			bindings.out_texture[dstCoord] = NVCLAMP(op);
		}
	}

#else

	////////////////////////////////////////////////////////////////////////////////////////////
	// [NVSharpen] methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//!< groupshared variables
	groupshared NVF shPixelsY[NIS_NIS_NUM_PIXEL_Y][NIS_NIS_NUM_PIXEL_X];

	// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L833-L846
	NVF CalcLTIFast(Config config, const NVF y[5]) {
		const NVF a_min = min(min(y[0], y[1]), y[2]);
		const NVF a_max = max(max(y[0], y[1]), y[2]);

		const NVF b_min = min(min(y[2], y[3]), y[4]);
		const NVF b_max = max(max(y[2], y[3]), y[4]);

		const NVF a_cont = a_max - a_min;
		const NVF b_cont = b_max - b_min;

		const NVF cont_ratio = max(a_cont, b_cont) / (min(a_cont, b_cont) + config.eps);
		return (1.0f - saturate((cont_ratio - config.min_contrast_ratio) * config.ratio_norm)) * config.contrast_boost;
	}

	// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L848-L860
	NVF EvalUSM(Config config, const NVF pxl[5], const NVF sharpnessStrength, const NVF sharpnessLimit) {
		// USM profile
		NVF y_usm = -0.6001f * pxl[1] + 1.2002f * pxl[2] - 0.6001f * pxl[3];
		// boost USM profile
		y_usm *= sharpnessStrength;
		// clamp to the limit
		y_usm = min(sharpnessLimit, max(-sharpnessLimit, y_usm));
		// reduce ringing
		y_usm *= CalcLTIFast(config, pxl);

		return y_usm;
	}

	// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L862-L914
	NVF4 GetDirUSM(Config config, const NVF p[5][5]) {
		// sharpness boost & limit are the same for all directions
		const NVF scaleY = 1.0f - saturate((p[2][2] - config.sharp_start_y) * config.sharp_scale_y);
		// scale the ramp to sharpen as a function of luma
		const NVF sharpnessStrength = scaleY * config.sharp_strength_scale + config.sharp_strength_min;
		// scale the ramp to limit USM as a function of luma
		const NVF sharpnessLimit = (scaleY * config.sharp_limit_scale + config.sharp_limit_min) * p[2][2];

		NVF4 rval;

		// 0 deg filter
		NVF interp0Deg[5];
		for (NVI i = 0; i < 5; ++i) {
			interp0Deg[i] = p[i][2];
		}

		rval.x = EvalUSM(config, interp0Deg, sharpnessStrength, sharpnessLimit);

		// 90 deg filter
		NVF interp90Deg[5];
		for (NVI i = 0; i < 5; ++i) {
			interp90Deg[i] = p[2][i];
		}

		rval.y = EvalUSM(config, interp90Deg, sharpnessStrength, sharpnessLimit);

		// 45 deg filter
		NVF interp45Deg[5];
		interp45Deg[0] = p[1][1];
		interp45Deg[1] = lerp(p[2][1], p[1][2], 0.5f);
		interp45Deg[2] = p[2][2];
		interp45Deg[3] = lerp(p[3][2], p[2][3], 0.5f);
		interp45Deg[4] = p[3][3];

		rval.z = EvalUSM(config, interp45Deg, sharpnessStrength, sharpnessLimit);

		// 135 deg filter
		NVF interp135Deg[5];
		interp135Deg[0] = p[3][1];
		interp135Deg[1] = lerp(p[3][2], p[2][1], 0.5f);
		interp135Deg[2] = p[2][2];
		interp135Deg[3] = lerp(p[2][3], p[1][2], 0.5f);
		interp135Deg[4] = p[1][3];

		rval.w = EvalUSM(config, interp135Deg, sharpnessStrength, sharpnessLimit);

		return rval;
	}

	// @reference https://github.com/NVIDIAGameWorks/NVIDIAImageScaling/blob/main/NIS/NIS_Scaler.h#L916-L1019
	void NVSharpen(Config config, Bindings bindings, NVU2 blockIdx, NVU threadIdx) {
		const NVI dstBlockX = NVI(NIS_BLOCK_WIDTH * blockIdx.x);
		const NVI dstBlockY = NVI(NIS_BLOCK_HEIGHT * blockIdx.y);

		// fill in input luma tile in batches of 2x2 pixels
		// we use texture gather to get extra support necessary
		// to compute 2x2 edge map outputs too
		const NVF kShift = 0.5f - NIS_SUPPORT_SIZE / 2;

		for (NVI i = NVI(threadIdx) * 2; i < NIS_NIS_NUM_PIXEL_X * NIS_NIS_NUM_PIXEL_Y / 2; i += NIS_THREAD_GROUP_SIZE * 2) {
			NVU2 pos = NVU2(NVU(i) % NVU(NIS_NIS_NUM_PIXEL_X), NVU(i) / NVU(NIS_NIS_NUM_PIXEL_X) * 2);

			[unroll]
			for (NVI dy = 0; dy < 2; dy++) {
				[unroll]
				for (NVI dx = 0; dx < 2; dx++) {
#if NIS_VIEWPORT_SUPPORT
					const NVF tx = (dstBlockX + pos.x + config.input_viewport_origin.x + dx + kShift) * config.src_norm.x;
					const NVF ty = (dstBlockY + pos.y + config.input_viewport_origin.y + dy + kShift) * config.src_norm.y;
#else
					const NVF tx = (dstBlockX + pos.x + dx + kShift) * config.src_norm.x;
					const NVF ty = (dstBlockY + pos.y + dy + kShift) * config.src_norm.y;
#endif

#if NIS_NV12_SUPPORT
					shPixelsY[pos.y + dy][pos.x + dx] = bindings.in_texture_y.SampleLevel(bindings.sampler_linear_clamp, NVF2(tx, ty), 0);
#else
					const NVF4 color = bindings.in_texture.SampleLevel(bindings.sampler_linear_clamp, NVF2(tx, ty), 0);
					shPixelsY[pos.y + dy][pos.x + dx] = getY(color.xyz);
#endif
				}
			}
		}

		GroupMemoryBarrierWithGroupSync();

		for (NVI k = NVI(threadIdx); k < NIS_BLOCK_WIDTH * NIS_BLOCK_HEIGHT; k += NIS_THREAD_GROUP_SIZE) {
			const NVI2 pos = NVI2(NVU(k) % NVU(NIS_BLOCK_WIDTH), NVU(k) / NVU(NIS_BLOCK_WIDTH));

			// load 5x5 support to regs
			NVF p[5][5];
			[unroll]
			for (NVI i = 0; i < 5; ++i) {
				[unroll]
				for (NVI j = 0; j < 5; ++j) {
					p[i][j] = shPixelsY[pos.y + i][pos.x + j];
				}
			}

			// get directional filter bank output
			NVF4 dirUSM = GetDirUSM(config, p);

			// generate weights for directional filters
			NVF4 w = GetEdgeMap(config, p, NIS_SUPPORT_SIZE / 2 - 1, NIS_SUPPORT_SIZE / 2 - 1);

			// final USM is a weighted sum filter outputs
			const NVF usmY = (dirUSM.x * w.x + dirUSM.y * w.y + dirUSM.z * w.z + dirUSM.w * w.w);

			// do bilinear tap and correct rgb texel so it produces new sharpened luma
			const NVI dstX = dstBlockX + pos.x;
			const NVI dstY = dstBlockY + pos.y;

#if NIS_VIEWPORT_SUPPORT
			if (NVU(dstX) > config.output_viewport_size.x || NVU(dstY) > config.output_viewport_size.y) {
				continue;
			}

			NVF2 coord    = NVF2((dstX + config.input_viewport_origin.x + 0.5f) * config.src_norm.x, (dstY + config.input_viewport_origin.y + 0.5f) * config.src_norm.y);
			NVU2 dstCoord = NVU2(dstX, dstY) + config.output_viewport_origin;
#else
			NVF2 coord    = NVF2((dstX + 0.5f) * config.src_norm.x, (dstY + 0.5f) * config.src_norm.y);
			NVU2 dstCoord = NVU2(dstX, dstY);
#endif

#if NIS_NV12_SUPPORT
			NVF  y  = bindings.in_texture_y.SampleLevel(bindings.sampler_linear_clamp, coord, 0);
			NVF2 uv = bindings.in_texture_uv.SampleLevel(bindings.sampler_linear_clamp, coord, 0);
			NVF4 op = NVF4(YUVtoRGB(NVF3(y, uv)), 1.0f);
#else
			NVF4 op = bindings.in_texture.SampleLevel(bindings.sampler_linear_clamp, coord, 0);
#endif

#if NIS_HDR_MODE == NIS_HDR_MODE_LINEAR
			const NVF kEps = 1e-4f * kHDRCompressionFactor * kHDRCompressionFactor;
			NVF newY = p[2][2] + usmY;
			newY = max(newY, 0.0f);
			const NVF oldY = p[2][2];
			const NVF corr = (newY * newY + kEps) / (oldY * oldY + kEps);
			op.xyz *= corr;
#else
			op.xyz += usmY;
#endif

			bindings.out_texture[dstCoord] = NVCLAMP(op);
		}
	}

#endif

}
