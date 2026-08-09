#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateWidget.h"

//* c++
#include <cstdint>
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
	// SizeRule enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief BoxPanel のスロットが主軸方向のサイズをどう決めるか.
	enum class SizeRule : uint8_t {
		Auto, //!< 子の希望サイズをそのまま使う.
		Fill, //!< 残り領域を比率で分け合う.
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Slot class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief BoxPanel のレイアウトスロット. フルエントに設定でき, Content() で子を割り当てる.
	class Slot final {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* slot option *//

		Slot& AutoSize() { rule_ = SizeRule::Auto; return *this; }

		Slot& Fill(float ratio) { rule_ = SizeRule::Fill; fill_ = ratio; return *this; }

		Slot& Content(WidgetPointer widget) { widget_ = std::move(widget); return *this; }

		//* getter *//

		SizeRule GetRule() const { return rule_; }

		float GetFillRatio() const { return fill_; }

		const WidgetPointer& GetWidget() const { return widget_; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		SizeRule rule_ = SizeRule::Auto;
		float fill_    = 1.0f;

		WidgetPointer widget_ = nullptr;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// BoxPanel class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 複数の子をスロット単位で縦/横に並べるレイアウトパネル.
	//! @note 各スロットは AutoSize() / Fill(比率) を持つ. 交差軸は常に Fill.
	class BoxPanel
		: public Widget {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		explicit BoxPanel(bool isVertical) noexcept : isVertical_(isVertical) {}

		//* slot option *//

		Slot& AddSlot();

		//* widget option *//

		Vector2f ComputeDesiredSize(float scale, ImGuiRenderer* renderer) const override;

		void OnArrangeChildren(const Geometry& allotted, ArrangedChildren& out) const override;

		int32_t OnPaint(const PaintArguments& arguments, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		bool isVertical_ = false;

		std::vector<std::unique_ptr<Slot>> slots_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// VerticalBox class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 子を縦に並べる BoxPanel. ウィンドウのクローム(タイトル行 / メニュー行 / 中身)に使う.
	class VerticalBox final
		: public BoxPanel {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		VerticalBox() noexcept : BoxPanel(true) {}

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
