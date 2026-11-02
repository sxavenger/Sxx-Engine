#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* SxGui
#include "SxGuiBasic.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Traits/Concept.h>
#include <Lib/Reflection/EnumUtil.h>
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
	// tree node methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief フレーム付きのツリーノードを表示する
	bool TreeNodeHeader(const std::string_view& label, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None);

	////////////////////////////////////////////////////////////////////////////////////////////
	// widget methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief 利用可能なコンテンツ領域の幅いっぱいにボタンを表示する
	bool ButtonStretchWidth(const std::string_view& label, float height = 0.0f);

	//! @brief 画像を表示領域の中央に表示する
	void ImageContentRect(const Graphics::Descriptor::Handle& handle, const ContentRect& rect);

	//! @brief 画像を表示領域の中央に表示する
	void ImageContentRect(const Graphics::Descriptor::Handle& handle, const ContentRect& rect, const Color4f& bg_color, const Color4f& tint_col = Color4f(0xFFFFFFFF));

	//! @brief 画像をアスペクト比を保ったまま表示領域の中央に表示する
	void ImageAspectFit(const Graphics::Descriptor::Handle& handle, const Vector2f& image_size, const Vector2f& display_size);

	//! @brief 画像をアスペクト比を保ったまま表示領域の中央に表示する
	void ImageAspectFit(const Graphics::Descriptor::Handle& handle, const Vector2f& image_size, const Vector2f& display_size, const Color4f& bg_color, const Color4f& tint_col = Color4f(0xFFFFFFFF));

	//! @brief 列挙型の値を選択するコンボボックスを表示する
	template <Concept::Enum T>
	bool ComboEnum(const std::string_view& label, T* v, ImGuiComboFlags flags = ImGuiComboFlags_None);

}

////////////////////////////////////////////////////////////////////////////////////////////
// SxGui (Extension) namespace template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Concept::Enum T>
bool SxGui::ComboEnum(const std::string_view& label, T* v, ImGuiComboFlags flags) {
	bool changed = false;

	if (SxGui::BeginCombo(label, EnumUtil<T>::GetName(*v), flags)) {
		for (const auto& [value, name] : magic_enum::enum_entries<T>()) {
			bool selected = *v == value;
			if (SxGui::Selectable(name.data(), &selected)) {
				*v = value;
				changed |= true;
			}
		}

		SxGui::EndCombo();
	}
	
	return changed;
}

SXAVENGER_ENGINE_NAMESPACE_END
