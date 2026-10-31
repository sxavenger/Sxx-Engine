//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* pass
#include "../PassContext.hlsli"

//* system
#include "../../System/Semantics.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//! @brief コンテキスト情報
ConstantBuffer<PassContext> Context : register(b0);

//! @brief 描画パス情報
cbuffer PassData : register(b1) {
}

//! @brief Lighting情報
Texture2D<float32_t4> gDirect   : register(t0);
Texture2D<float32_t4> gIndirect : register(t1);

//! @brief Scene情報
RWTexture2D<float32_t4> gScene : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("compute")]
[numthreads(8, 8, 1)]
void main(Semantics::Compute semantics) {

	uint32_t2 pixel = semantics.dispatch_thread_id.xy;

	if (!Context.IsWithinResolution(pixel)) {
		return; //!< 画面外のpixelは処理しない
	}

	float32_t3 direct_color   = gDirect[pixel].rgb;
	float32_t3 indirect_color = gIndirect[pixel].rgb;
	// todo: 各Lightingの係数の追加をする.

	float32_t direct_transparency   = gDirect[pixel].a;
	float32_t indirect_transparency = gIndirect[pixel].a;

	float32_t3 color       = direct_color + indirect_color; //!< lo同士は加算で合成.
	float32_t transparency = max(direct_transparency, indirect_transparency); //!< alphaはmaxで合成.

	gScene[pixel] = float32_t4(color, transparency);

}
