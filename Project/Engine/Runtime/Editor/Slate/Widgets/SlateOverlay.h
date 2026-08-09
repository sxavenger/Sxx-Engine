#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateWidget.h"

//* c++
#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Overlay class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 子を重ねて配置するパネル. 後に追加したものほど手前(最後が最前面)になる.
	//! @note フローティングウィンドウで, ドックのタブ行の上にウィンドウ操作ボタン
	//!       (最小化 / 最大化 / 閉じる)を重ねる用途を想定する.
	class Overlay final
		: public Widget {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// OverlaySlot structure
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief Overlay のレイアウトスロット. widget / hAlign / vAlign を持つ.
		//! @note 型名を Slate:: で修飾しているのは, メンバ関数名 HAlign() / VAlign() が
		//!       クラススコープ内で同名の型を隠してしまうため.
		struct OverlaySlot {
		public:

			//=========================================================================================
			// public methods
			//=========================================================================================

			OverlaySlot& HAlign(Slate::HAlign align) { hAlign = align; return *this; }

			OverlaySlot& VAlign(Slate::VAlign align) { vAlign = align; return *this; }

			OverlaySlot& Content(WidgetPointer content) { widget = std::move(content); return *this; }

			//=========================================================================================
			// public variables
			//=========================================================================================

			WidgetPointer widget = nullptr;

			Slate::HAlign hAlign = Slate::HAlign::Fill;
			Slate::VAlign vAlign = Slate::VAlign::Fill;

		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* slot option *//

		OverlaySlot& AddSlot();

		//* widget option *//

		Vector2f ComputeDesiredSize(float scale, ImGuiRenderer* renderer) const override;

		void OnArrangeChildren(const Geometry& allotted, ArrangedChildren& out) const override;

		int32_t OnPaint(const PaintArguments& arguments, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::vector<std::unique_ptr<OverlaySlot>> slots_;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
