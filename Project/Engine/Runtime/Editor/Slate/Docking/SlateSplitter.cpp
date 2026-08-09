#include "SlateSplitter.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
// Splitter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Slate::Splitter::CollectPanels(std::vector<DockPanelPointer>& out) const {
	if (auto* node = dynamic_cast<const IDockNode*>(first_.get())) {
		node->CollectPanels(out);
	}
	if (auto* node = dynamic_cast<const IDockNode*>(second_.get())) {
		node->CollectPanels(out);
	}
}

Vector2f Slate::Splitter::ComputeDesiredSize(float scale, ImGuiRenderer* renderer) const {
	Vector2f a{}, b{};

	if (first_) {
		a = first_->ComputeDesiredSize(scale, renderer);
		first_->CacheDesiredSize(a);
	}
	if (second_) {
		b = second_->ComputeDesiredSize(scale, renderer);
		second_->CacheDesiredSize(b);
	}

	if (orientation_ == Orientation::Horizontal) {
		return { a.x + b.x + DividerSize(), (a.y > b.y ? a.y : b.y) };
	}

	return { (a.x > b.x ? a.x : b.x), a.y + b.y + DividerSize() };
}

void Slate::Splitter::OnArrangeChildren(const Geometry& allotted, ArrangedChildren& out) const {
	const bool horizontal = (orientation_ == Orientation::Horizontal);
	const float total  = horizontal ? allotted.localSize.x : allotted.localSize.y;
	const float usable = total - DividerSize();

	if (usable <= 0.0f) {
		return;
	}

	//!< windowを縮めた場合や, 復元した比率が極端な場合もここで丸める.
	//!< note: ratio_は書き換えない. windowを広げたら元の比率へ戻るようにするため.
	const float ratio      = ClampWithSize(ratio_, usable);
	const float firstSize  = usable * ratio;
	const float secondSize = usable - firstSize;

	if (first_) {
		const Vector2f offset{ 0.0f, 0.0f };
		const Vector2f size = horizontal ? Vector2f{ firstSize, allotted.localSize.y }
										  : Vector2f{ allotted.localSize.x, firstSize };
		out.push_back({ first_, allotted.MakeChild(offset, size) });
	}
	if (second_) {
		const Vector2f offset = horizontal ? Vector2f{ firstSize + DividerSize(), 0.0f }
											: Vector2f{ 0.0f, firstSize + DividerSize() };
		const Vector2f size = horizontal ? Vector2f{ secondSize, allotted.localSize.y }
										  : Vector2f{ allotted.localSize.x, secondSize };
		out.push_back({ second_, allotted.MakeChild(offset, size) });
	}
}

Slate::Geometry Slate::Splitter::DividerGeometry(const Geometry& allotted) const {
	const bool horizontal = (orientation_ == Orientation::Horizontal);
	const float total      = horizontal ? allotted.localSize.x : allotted.localSize.y;
	const float usable     = total - DividerSize();

	//!< OnArrangeChildrenと同じ丸めを通す. ここだけ生のratio_を使うと,
	//!< 仕切りの描画位置と子の分割位置がずれ, 掴める場所も食い違う.
	const float firstSize  = usable * ClampWithSize(ratio_, usable);

	const Vector2f offset = horizontal ? Vector2f{ firstSize, 0.0f } : Vector2f{ 0.0f, firstSize };
	const Vector2f size   = horizontal ? Vector2f{ DividerSize(), allotted.localSize.y }
										: Vector2f{ allotted.localSize.x, DividerSize() };
	return allotted.MakeChild(offset, size);
}

Slate::Geometry Slate::Splitter::DividerHitGeometry(const Geometry& allotted) const {
	const Geometry divider = DividerGeometry(allotted);
	const float extra = GrabSize() - DividerSize();

	if (extra <= 0.0f) {
		return divider;
	}

	const bool horizontal = (orientation_ == Orientation::Horizontal);
	const float half = extra * 0.5f;

	return Geometry{
		horizontal ? Vector2f{ divider.absolutePosition.x - half, divider.absolutePosition.y }
				   : Vector2f{ divider.absolutePosition.x, divider.absolutePosition.y - half },
		horizontal ? Vector2f{ divider.localSize.x + extra, divider.localSize.y }
				   : Vector2f{ divider.localSize.x, divider.localSize.y + extra },
		divider.scale };
}

int32_t Slate::Splitter::OnPaint(const PaintArguments& arguments, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const {
	int32_t maxLayer = layer;
	ArrangedChildren arranged;
	OnArrangeChildren(geometry, arranged);

	for (const auto& child : arranged) {
		const int32_t childLayer = child.widget->OnPaint(arguments, child.geometry, renderer, layer + 1);
		maxLayer = std::max(maxLayer, childLayer);
	}

	PaintDivider(renderer, geometry);
	return maxLayer;
}

Slate::Cursor Slate::Splitter::GetCursor(const Geometry& geometry, Vector2f absolutePosition) const {
	if (!dragging_ && !DividerHitGeometry(geometry).ContainsAbsolute(absolutePosition)) {
		return Cursor::Default;
	}

	return (orientation_ == Orientation::Horizontal) ? Cursor::ResizeLeftRight : Cursor::ResizeUpDown;
}

Slate::Reply Slate::Splitter::OnMouseButtonDown(const Geometry& geometry, const PointerEvent& event) {
	if (event.button != PointerEvent::Button::Left) {
		return Reply::Unhandled();
	}

	const Geometry divider = DividerGeometry(geometry);
	if (!DividerHitGeometry(geometry).ContainsAbsolute(event.screenPosition)) {
		return Reply::Unhandled();
	}

	//!< 掴んだ位置と仕切り先頭のズレを覚えておく.
	//!< これが無いと押した瞬間に仕切りがカーソル位置へ飛び,
	//!< 以降カーソルが仕切りから外れやすくなる(細い仕切りで顕著).
	const bool horizontal = (orientation_ == Orientation::Horizontal);
	grabOffset_ = horizontal ? (event.screenPosition.x - divider.absolutePosition.x)
							  : (event.screenPosition.y - divider.absolutePosition.y);
	dragging_ = true;
	return Reply::Handled().CaptureMouse(shared_from_this());
}

Slate::Reply Slate::Splitter::OnMouseMove(const Geometry& geometry, const PointerEvent& event) {
	if (!dragging_) {
		//!< ホバー表示のため, 仕切りの上にいるかを記録する.
		hovered_ = DividerHitGeometry(geometry).ContainsAbsolute(event.screenPosition);
		return hovered_ ? Reply::Handled() : Reply::Unhandled();
	}

	const bool horizontal = (orientation_ == Orientation::Horizontal);
	const float total  = horizontal ? geometry.localSize.x : geometry.localSize.y;
	const float usable = total - DividerSize();

	if (usable <= 0.0f) {
		return Reply::Handled();
	}

	const float local = horizontal ? (event.screenPosition.x - geometry.absolutePosition.x)
									: (event.screenPosition.y - geometry.absolutePosition.y);
	//!< 掴んだ位置のズレを差し引く(仕切りが飛ばない).
	//!< note: 比率の下限だけだと片側を数pxまで潰せてしまい, タブが表示できなくなる.
	ratio_ = ClampWithSize((local - grabOffset_) / usable, usable);
	return Reply::Handled();
}

Slate::Reply Slate::Splitter::OnMouseButtonUp(const Geometry& /*geometry*/, const PointerEvent& /*event*/) {
	if (!dragging_) {
		return Reply::Unhandled();
	}

	dragging_ = false;
	return Reply::Handled().ReleaseMouseCapture();
}

float Slate::Splitter::ClampWithSize(float ratio, float usable) {

	const float clamped = Clamp(ratio);

	if (usable <= 0.0f) {
		return clamped;
	}

	//!< 両側にkMinChildSizeを確保できないほど狭い場合は, 比率の下限だけで妥協する.
	//!< (半分ずつに割るしかない)
	if (usable < kMinChildSize * 2.0f) {
		return clamped;
	}

	const float minRatio = kMinChildSize / usable;
	const float maxRatio = 1.0f - minRatio;

	if (clamped < minRatio) {
		return minRatio;
	}
	if (clamped > maxRatio) {
		return maxRatio;
	}

	return clamped;
}

float Slate::Splitter::Clamp(float ratio) {
	if (ratio < kMinRatio) {
		return kMinRatio;
	}
	if (ratio > 1.0f - kMinRatio) {
		return 1.0f - kMinRatio;
	}
	return ratio;
}

void Slate::Splitter::PaintDivider(ImGuiRenderer* renderer, const Geometry& geometry) const {
	const Color4f fill = dragging_ ? Style::GetStyle().colors.active
					   : hovered_  ? Style::GetStyle().colors.hover
								   : Style::GetStyle().colors.background;
	renderer->DrawRect(DividerGeometry(geometry), fill);
}
