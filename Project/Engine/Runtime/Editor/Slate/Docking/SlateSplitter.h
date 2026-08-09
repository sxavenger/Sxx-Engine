#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateDockPanel.h"

//* c++
#include <cstdint>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Editor)

////////////////////////////////////////////////////////////////////////////////////////////
// Slate namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Slate {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Splitter class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 2つの子を縦または横に分割し, ドラッグ可能な仕切りで比率を変えるノード.
	class Splitter final
		: public Widget, public IDockNode {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Orientation enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief Horizontal = 子を左右に並べる / Vertical = 子を上下に並べる.
		enum class Orientation : uint8_t {
			Horizontal,
			Vertical
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		Splitter() noexcept = default;

		Splitter(Orientation orientation, WidgetPointer first, WidgetPointer second, float ratio = 0.5f) noexcept
			: orientation_(orientation), first_(std::move(first)), second_(std::move(second)), ratio_(ratio) {
		}

		//* static parameter *//

		//!< 仕切りの見た目の幅. スタイルから取るので定数にしない.
		static float DividerSize() { return Style::GetStyle().metrics.dividerSize; }

		//! @brief 掴める幅の下限. 見た目を細くしても操作できるようにする
		//!        (UE5 も見た目より広い当たり判定を持つ).
		static float GrabSize() { const float size = DividerSize(); return (size < kMinGrabSize) ? kMinGrabSize : size; }

		//* splitter option *//

		void SetOrientation(Orientation orientation) { orientation_ = orientation; }

		void SetChildren(WidgetPointer first, WidgetPointer second) { first_ = std::move(first); second_ = std::move(second); }

		void SetRatio(float ratio) { ratio_ = Clamp(ratio); }

		float GetRatio() const { return ratio_; }

		Orientation GetOrientation() const { return orientation_; }

		const WidgetPointer& First() const { return first_; }

		const WidgetPointer& Second() const { return second_; }

		void CollectPanels(std::vector<DockPanelPointer>& out) const override;

		//* geometry option *//

		//! @brief 仕切りの矩形(絶対座標).
		Geometry DividerGeometry(const Geometry& allotted) const;

		//! @brief 当たり判定用の矩形(絶対座標). 見た目が細くても掴めるよう左右(上下)に広げる.
		Geometry DividerHitGeometry(const Geometry& allotted) const;

		//* widget option *//

		Vector2f ComputeDesiredSize(float scale, ImGuiRenderer* renderer) const override;

		void OnArrangeChildren(const Geometry& allotted, ArrangedChildren& out) const override;

		int32_t OnPaint(const PaintArguments& arguments, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const override;

		//* pointer option *//

		Cursor GetCursor(const Geometry& geometry, Vector2f absolutePosition) const override;

		void OnMouseLeave() override { hovered_ = false; }

		Reply OnMouseButtonDown(const Geometry& geometry, const PointerEvent& event) override;

		Reply OnMouseMove(const Geometry& geometry, const PointerEvent& event) override;

		Reply OnMouseButtonUp(const Geometry& geometry, const PointerEvent& event) override;

		//=========================================================================================
		// public variables
		//=========================================================================================

		//!< 掴める幅の下限.
		static constexpr float kMinGrabSize = 6.0f;

		static constexpr float kMinRatio = 0.05f;

		//!< 片側に必ず残すピクセル幅の下限.
		//!< 比率だけの下限だと入れ子のsplitterで掛け算になり, 数pxまで潰せてしまう.
		//!< そこまで狭めるとタブバーにタブが1枚も置けず, タブを切り替える手段が失われる.
		//!< note: DockTabStack::kTabMinShrunkWidth(28) + 余白 + × が収まる幅にしている.
		static constexpr float kMinChildSize = 96.0f;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		Orientation orientation_ = Orientation::Horizontal;

		WidgetPointer first_;
		WidgetPointer second_;
		float ratio_ = 0.5f;

		mutable bool dragging_ = false;
		mutable bool hovered_  = false;

		mutable float grabOffset_ = 0.0f; //!< 掴んだ位置と仕切り先頭のズレ.

		//=========================================================================================
		// private methods
		//=========================================================================================

		static float Clamp(float ratio);

		//! @brief 使える幅が分かっているときの比率のクランプ.
		//! @note 比率の下限とピクセルの下限の両方を満たす値へ丸める.
		static float ClampWithSize(float ratio, float usable);

		//! @brief 仕切りの描画. 装飾は付けず, 状態を色だけで示す.
		//!        通常: 背景色(パネル間の溝として見える) / ホバー: 明るいグレー / ドラッグ: 青.
		//! @note 掴める位置はカーソル形状(左右/上下矢印)でも分かる.
		void PaintDivider(ImGuiRenderer* renderer, const Geometry& geometry) const;

	};

	using SplitterPointer = std::shared_ptr<Splitter>;

}

SXAVENGER_ENGINE_NAMESPACE_END
