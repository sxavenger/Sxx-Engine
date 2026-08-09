#include "SlateDocking.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// dock zone methods
////////////////////////////////////////////////////////////////////////////////////////////

void Slate::BuildDockZoneMarkers(const Geometry& area, DockZoneMarker out[kDockZoneMarkerCount]) {
	const float minSide = (area.localSize.x < area.localSize.y) ? area.localSize.x : area.localSize.y;

	//!< マーカー3つ分(中央＋左右)が収まるようにサイズを決める.
	float m = kDockZoneMarkerSize;
	const float needed = kDockZoneMarkerSize * 3.0f + kDockZoneMarkerGap * 2.0f;

	if (minSide > 0.0f && minSide < needed) {
		m = minSide / 3.5f;
	}

	//!< 分割されたスタックは矩形が小さく, 上の式だとマーカーが数pxまで縮んで狙えなくなる.
	//!< 隙間を詰めてでも掴める大きさを保つ. 領域より大きくなる場合だけ更に縮める.
	if (m < kDockZoneMarkerMinSize) {
		m = kDockZoneMarkerMinSize;
	}

	if (minSide > 0.0f && m * 3.0f > minSide) {
		m = minSide / 3.0f; //!< それでも収まらないなら領域に合わせる. (隙間は0になる)
	}

	//!< マーカーが小さいときは隙間も詰める. 中央と上下左右が離れすぎると狙いづらい.
	const float gapScale = (m < kDockZoneMarkerSize) ? (m / kDockZoneMarkerSize) : 1.0f;
	const float gap      = m + gapScale * kDockZoneMarkerGap;
	const Vector2f center{ area.absolutePosition.x + area.localSize.x * 0.5f,
							area.absolutePosition.y + area.localSize.y * 0.5f };

	const DockZone zones[kDockZoneMarkerCount] = {
		DockZone::Center, DockZone::Left, DockZone::Right,
		DockZone::Top,    DockZone::Bottom
	};
	const Vector2f centers[kDockZoneMarkerCount] = {
		center,
		{ center.x - gap, center.y },
		{ center.x + gap, center.y },
		{ center.x, center.y - gap },
		{ center.x, center.y + gap },
	};

	for (int32_t i = 0; i < kDockZoneMarkerCount; ++i) {
		out[i].zone = zones[i];
		out[i].box  = Geometry{ { centers[i].x - m * 0.5f, centers[i].y - m * 0.5f }, { m, m }, area.scale };
	}
}

Slate::DockZone Slate::HitTestDockZoneMarkers(const Geometry& area, Vector2f point) {
	if (area.localSize.x <= 0.0f || area.localSize.y <= 0.0f) {
		return DockZone::None;
	}

	DockZoneMarker markers[kDockZoneMarkerCount];
	BuildDockZoneMarkers(area, markers);

	//!< 当たり判定は見た目より広げる. 見た目どおりの矩形だと縁で外して狙いづらい.
	//!< note: 中央のマーカーを優先したいので, 判定は BuildDockZoneMarkers の順序
	//!<       (Center -> Left -> Right -> Top -> Bottom) のまま前から見る.
	for (const DockZoneMarker& marker : markers) {

		const Geometry grab = Geometry{
			{ marker.box.absolutePosition.x - kDockZoneMarkerGrabPadding,
			  marker.box.absolutePosition.y - kDockZoneMarkerGrabPadding },
			{ marker.box.localSize.x + kDockZoneMarkerGrabPadding * 2.0f,
			  marker.box.localSize.y + kDockZoneMarkerGrabPadding * 2.0f },
			marker.box.scale
		};

		if (grab.ContainsAbsolute(point)) {
			return marker.zone;
		}
	}

	return DockZone::None;
}

Slate::Geometry Slate::DockZonePreviewGeometry(const Geometry& area, DockZone zone) {
	const Vector2f p = area.absolutePosition;
	const Vector2f s = area.localSize;

	switch (zone) {
		case DockZone::Left:   return Geometry{ p, { s.x * 0.5f, s.y }, area.scale };
		case DockZone::Right:  return Geometry{ { p.x + s.x * 0.5f, p.y }, { s.x * 0.5f, s.y }, area.scale };
		case DockZone::Top:    return Geometry{ p, { s.x, s.y * 0.5f }, area.scale };
		case DockZone::Bottom: return Geometry{ { p.x, p.y + s.y * 0.5f }, { s.x, s.y * 0.5f }, area.scale };
		case DockZone::Center: return area;
		default:               return Geometry{};
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// dock tree methods
////////////////////////////////////////////////////////////////////////////////////////////

Slate::DockTabStackPointer Slate::FindFirstTabStack(const WidgetPointer& root) {
	if (!root) {
		return nullptr;
	}
	if (auto stack = std::dynamic_pointer_cast<DockTabStack>(root)) {
		return stack;
	}
	if (auto split = std::dynamic_pointer_cast<Splitter>(root)) {
		if (auto stack = FindFirstTabStack(split->First())) {
			return stack;
		}
		if (auto stack = FindFirstTabStack(split->Second())) {
			return stack;
		}
	}
	return nullptr;
}

std::vector<Slate::DockPanelPointer> Slate::CollectAllPanels(const WidgetPointer& root) {
	std::vector<DockPanelPointer> out;

	if (auto* node = dynamic_cast<const IDockNode*>(root.get())) {
		node->CollectPanels(out);
	}

	return out;
}

Slate::WidgetPointer Slate::DockInto(WidgetPointer targetRoot, const std::vector<DockPanelPointer>& incoming, DockZone zone) {
	if (incoming.empty() || zone == DockZone::None) {
		return targetRoot;
	}

	//!< 中央: 既存スタックへタブ追加(無ければ新規スタックを root にする).
	if (zone == DockZone::Center) {
		if (auto stack = FindFirstTabStack(targetRoot)) {
			stack->AddPanels(incoming);
			return targetRoot;
		}
		auto stack = std::make_shared<DockTabStack>();
		stack->AddPanels(incoming);
		return stack;
	}

	//!< 方向: 新しいタブスタックを作って分割で連結.
	auto incomingStack = std::make_shared<DockTabStack>();
	incomingStack->AddPanels(incoming);

	if (!targetRoot) {
		return incomingStack;
	}

	const bool horizontal    = (zone == DockZone::Left || zone == DockZone::Right);
	const auto orientation   = horizontal ? Splitter::Orientation::Horizontal : Splitter::Orientation::Vertical;
	const bool incomingFirst = (zone == DockZone::Left || zone == DockZone::Top);

	return std::make_shared<Splitter>(
		orientation,
		incomingFirst ? WidgetPointer(incomingStack) : targetRoot,
		incomingFirst ? targetRoot : WidgetPointer(incomingStack),
		0.5f);
}

Slate::WidgetPointer Slate::ReplaceNode(const WidgetPointer& root, const WidgetPointer& oldNode, const WidgetPointer& newNode) {
	if (!root) {
		return nullptr;
	}
	if (root == oldNode) {
		return newNode;
	}
	if (auto split = std::dynamic_pointer_cast<Splitter>(root)) {
		split->SetChildren(ReplaceNode(split->First(), oldNode, newNode), ReplaceNode(split->Second(), oldNode, newNode));
	}
	return root;
}

////////////////////////////////////////////////////////////////////////////////////////////
// dock target methods
////////////////////////////////////////////////////////////////////////////////////////////

bool Slate::FindDockTargetAt(const WidgetPointer& node, const Geometry& geometry, Vector2f point, DockTarget& out) {
	if (!node || !geometry.ContainsAbsolute(point)) {
		return false;
	}

	if (auto stack = std::dynamic_pointer_cast<DockTabStack>(node)) {
		out.stack = stack;
		out.area  = geometry;
		return true;
	}

	//!< スプリッタなら子の配置を求めて降りる.
	Widget::ArrangedChildren children;
	node->OnArrangeChildren(geometry, children);

	for (const auto& child : children) {
		if (FindDockTargetAt(child.widget, child.geometry, point, out)) {
			return true;
		}
	}

	return false;
}

Slate::WidgetPointer Slate::DockIntoTarget(WidgetPointer root, const DockTarget& target, const std::vector<DockPanelPointer>& incoming) {
	if (incoming.empty() || target.zone == DockZone::None) {
		return root;
	}

	//!< 対象スタックが無い(空のウィンドウ等)ならツリー全体に対して結合する.
	if (!target.stack) {
		return DockInto(root, incoming, target.zone);
	}

	if (target.zone == DockZone::Center) {
		target.stack->AddPanels(incoming);
		return root;
	}

	auto incomingStack = std::make_shared<DockTabStack>();
	incomingStack->AddPanels(incoming);

	const bool horizontal    = (target.zone == DockZone::Left || target.zone == DockZone::Right);
	const bool incomingFirst = (target.zone == DockZone::Left || target.zone == DockZone::Top);

	auto split = std::make_shared<Splitter>(
		horizontal ? Splitter::Orientation::Horizontal : Splitter::Orientation::Vertical,
		incomingFirst ? WidgetPointer(incomingStack) : WidgetPointer(target.stack),
		incomingFirst ? WidgetPointer(target.stack)  : WidgetPointer(incomingStack),
		0.5f);

	return ReplaceNode(root, target.stack, split);
}

Slate::WidgetPointer Slate::DockSubtreeIntoTarget(WidgetPointer root, const DockTarget& target, WidgetPointer incoming) {
	if (!incoming || target.zone == DockZone::None) {
		return root;
	}

	//!< 中央 = タブ結合. 分割は保持できないのでパネルを平坦化する.
	if (target.zone == DockZone::Center) {
		return DockIntoTarget(root, target, CollectAllPanels(incoming));
	}

	//!< 対象スタックが無い(空のウィンドウ)ならツリーごと置き換える.
	if (!target.stack) {
		if (!root) {
			return incoming;
		}

		const bool horizontal = (target.zone == DockZone::Left || target.zone == DockZone::Right);
		const bool first      = (target.zone == DockZone::Left || target.zone == DockZone::Top);

		return std::make_shared<Splitter>(
			horizontal ? Splitter::Orientation::Horizontal : Splitter::Orientation::Vertical,
			first ? incoming : root,
			first ? root : incoming,
			0.5f);
	}

	//!< 方向 = 対象スタックの位置を「対象 + incoming」の分割に差し替える.
	//!< incoming の内部構造(分割・タブ)はそのまま維持される.
	const bool horizontal    = (target.zone == DockZone::Left || target.zone == DockZone::Right);
	const bool incomingFirst = (target.zone == DockZone::Left || target.zone == DockZone::Top);

	auto split = std::make_shared<Splitter>(
		horizontal ? Splitter::Orientation::Horizontal : Splitter::Orientation::Vertical,
		incomingFirst ? incoming : WidgetPointer(target.stack),
		incomingFirst ? WidgetPointer(target.stack) : incoming,
		0.5f);

	return ReplaceNode(root, target.stack, split);
}

////////////////////////////////////////////////////////////////////////////////////////////
// dock host methods
////////////////////////////////////////////////////////////////////////////////////////////

bool Slate::IsPointOnAnyTab(const WidgetPointer& root, Vector2f absolutePosition) {
	if (!root) {
		return false;
	}
	if (auto stack = std::dynamic_pointer_cast<DockTabStack>(root)) {
		return stack->ContainsTabAt(absolutePosition);
	}
	if (auto split = std::dynamic_pointer_cast<Splitter>(root)) {
		return IsPointOnAnyTab(split->First(), absolutePosition) || IsPointOnAnyTab(split->Second(), absolutePosition);
	}
	return false;
}

Slate::DockTabStackPointer Slate::FindTopRightTabStack(const WidgetPointer& root) {
	if (!root) {
		return nullptr;
	}
	if (auto stack = std::dynamic_pointer_cast<DockTabStack>(root)) {
		return stack;
	}
	if (auto split = std::dynamic_pointer_cast<Splitter>(root)) {
		const bool horizontal = (split->GetOrientation() == Splitter::Orientation::Horizontal);
		return FindTopRightTabStack(horizontal ? split->Second() : split->First());
	}
	return nullptr;
}

void Slate::ResetTabBarInsets(const WidgetPointer& root) {
	if (!root) {
		return;
	}
	if (auto stack = std::dynamic_pointer_cast<DockTabStack>(root)) {
		stack->SetTabBarRightInset(0.0f);
		return;
	}
	if (auto split = std::dynamic_pointer_cast<Splitter>(root)) {
		ResetTabBarInsets(split->First());
		ResetTabBarInsets(split->Second());
	}
}

void Slate::ApplyDockingHost(const WidgetPointer& root, IDockingHost* host) {
	if (!root) {
		return;
	}
	if (auto stack = std::dynamic_pointer_cast<DockTabStack>(root)) {
		stack->SetHost(host);
		return;
	}
	if (auto split = std::dynamic_pointer_cast<Splitter>(root)) {
		ApplyDockingHost(split->First(), host);
		ApplyDockingHost(split->Second(), host);
	}
}

Slate::WidgetPointer Slate::PruneEmptyNodes(const WidgetPointer& root) {
	if (!root) {
		return nullptr;
	}
	if (auto stack = std::dynamic_pointer_cast<DockTabStack>(root)) {
		return stack->IsEmpty() ? nullptr : root;
	}
	if (auto split = std::dynamic_pointer_cast<Splitter>(root)) {
		WidgetPointer a = PruneEmptyNodes(split->First());
		WidgetPointer b = PruneEmptyNodes(split->Second());

		if (!a && !b) {
			return nullptr;
		}
		if (!a) {
			return b; //!< 片方だけ残ったら分割をやめる.
		}
		if (!b) {
			return a;
		}

		split->SetChildren(a, b);
		return root;
	}
	return root; //!< ドックノード以外はそのまま.
}

bool Slate::RemovePanelFromTree(const WidgetPointer& root, const DockPanelPointer& panel) {
	if (!root || !panel) {
		return false;
	}
	if (auto stack = std::dynamic_pointer_cast<DockTabStack>(root)) {
		return stack->RemovePanel(panel);
	}
	if (auto split = std::dynamic_pointer_cast<Splitter>(root)) {
		return RemovePanelFromTree(split->First(), panel) || RemovePanelFromTree(split->Second(), panel);
	}
	return false;
}
