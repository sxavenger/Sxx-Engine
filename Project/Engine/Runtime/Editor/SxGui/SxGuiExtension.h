#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* SxGui
#include "SxGuiBasic.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Vector2.h>
#include <Lib/Math/Color3.h>
#include <Lib/Math/Color4.h>

//* c++
#include <string_view>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui (Extension) namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SxGui {

	////////////////////////////////////////////////////////////////////////////////////////////
	// ContentRect structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ContentRect {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Vector2f cursol; //!< カーソル位置
		Vector2f size;   //!< コンテンツ領域のサイズ

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// layout methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief 画像のアスペクト比を保って表示領域に収める矩形を計算する
	ContentRect CalcAspectFitRect(const Vector2f& image_size, const Vector2f& display_size);

	////////////////////////////////////////////////////////////////////////////////////////////
	// widget option methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief 利用可能なコンテンツ領域の幅いっぱいにボタンを表示する
	bool ButtonStretchWidth(const std::string_view& label, float height = 0.0f);

	//! @brief 画像をアスペクト比を保ったまま表示領域の中央に表示する
	void ImageAspectFit(const Graphics::Descriptor::Handle& handle, const Vector2f& image_size, const Vector2f& display_size);

	//! @brief 画像をアスペクト比を保ったまま表示領域の中央に表示する
	void ImageAspectFit(const Graphics::Descriptor::Handle& handle, const Vector2f& image_size, const Vector2f& display_size, const Color4f& bg_color, const Color4f& tint_col = Color4f(0xFFFFFFFF));

}

SXAVENGER_ENGINE_NAMESPACE_END
