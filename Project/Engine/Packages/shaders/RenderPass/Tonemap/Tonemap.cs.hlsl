//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* pass
#include "../PassContext.hlsli"

//* system
#include "../../System/Semantics.hlsli"

//* lib
#include "../../Lib/ACES/ColorSpace.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//! @brief コンテキスト情報
ConstantBuffer<PassContext> Context : register(b0);

//! @brief 描画パス情報
cbuffer PassData : register(b1) {
}

//! @btief Color情報
RWTexture2D<float32_t4> gSceneColor : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("compute")]
[numthreads(8, 8, 1)]
void main(Semantics::Compute semantics) {

	if (!Context.IsWithinResolution(semantics.dispatch_thread_id.xy)) {
		return; //!< 画面外のpixelは処理しない
	}

	float32_t3 color       = gSceneColor[semantics.dispatch_thread_id.xy].rgb;
	float32_t transparency = gSceneColor[semantics.dispatch_thread_id.xy].a;

	[branch]
	switch (Context.working_color_space) {
		case WorkingColorSpace::ACEScg:
			{ //!< Output Device Transform (ODT)

				// TODO: Rec709以外も対応
				const ACES::ODTParams p = ACES::ODTParams::Init(ACES::ODT::sRGB_100nits_dim);

				float32_t3 aces2065_1 = ACES::AP1ToAP0(color); //!< ACEScg -> ACES2065-1
				float32_t3 oces = ACES::RRTMain(aces2065_1);   //!< ACES2065-1 -> OCES
				float32_t3 linearCV = ACES::ODTFwd(oces, p);   //!< OCES -> linearCV

				color = linearCV;

				[branch]
				if (Context.display_encoder == DisplayEncoder::Software) {
					//!< ソフトウェア EOTF (Rec709) を適用する場合
					color = ACES::EOTFInv(linearCV, p.eotf, p.Y_MAX, p.Y_MIN);
				}
			}
			break;

		default:
			return; //!< Linearの場合は何もしない
	}

	gSceneColor[semantics.dispatch_thread_id.xy] = float32_t4(color, transparency);

}
