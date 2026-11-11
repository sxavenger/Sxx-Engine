#include "SlateDocking.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <algorithm>
#include <cstddef>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// anonymous namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	//!< マーカーを外していてもdropできるようにする縁の割合.
	//!< 領域の端からこの比率までが上下左右, それより内側は中央(tabとして追加).
	constexpr float kDockEdgeRatio = 0.28f;

	//! @brief tree内の全DockTabStackを First -> Second の順に訪ねる.
	//! @param function bool(const DockTabStackPointer&). trueを返すと走査を打ち切る.
	//! @retval 打ち切られたかどうか.
	template <class F>
	bool VisitTabStacks(const Slate::WidgetPointer& node, F&& function) {
		if (node == nullptr) {
			return false;
		}
		if (auto stack = std::dynamic_pointer_cast<Slate::DockTabStack>(node)) {
			return function(stack);
		}
		if (auto split = std::dynamic_pointer_cast<Slate::Splitter>(node)) {
			return VisitTabStacks(split->First(), function) || VisitTabStacks(split->Second(), function);
		}
		return false; //!< ドックノード以外は辿らない.
	}

	//! @brief zoneの向きに合わせて incoming と existing を分割で連結する.
	//! @note Left / Top は incoming が先(First), Right / Bottom は existing が先.
	Slate::WidgetPointer MakeSplitForZone(Slate::DockZone zone, const Slate::WidgetPointer& incoming, const Slate::WidgetPointer& existing) {
		const bool horizontal    = (zone == Slate::DockZone::Left || zone == Slate::DockZone::Right);
		const bool incomingFirst = (zone == Slate::DockZone::Left || zone == Slate::DockZone::Top);

		return std::make_shared<Slate::Splitter>(
			horizontal ? Slate::Splitter::Orientation::Horizontal : Slate::Splitter::Orientation::Vertical,
			incomingFirst ? incoming : existing,
			incomingFirst ? existing : incoming,
			0.5f);
	}

}

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
	DockTabStackPointer result = nullptr;
	VisitTabStacks(root, [&result](const DockTabStackPointer& stack) { result = stack; return true; });
	return result;
}

std::vector<Slate::DockPanelPointer> Slate::CollectAllPanels(const WidgetPointer& root) {
	std::vector<DockPanelPointer> out;

	if (auto* node = dynamic_cast<const IDockNode*>(root.get())) {
		node->CollectPanels(out);
	}

	return out;
}

bool Slate::ContainsPanel(const WidgetPointer& root, const DockPanelPointer& panel) {
	if (panel == nullptr) {
		return false;
	}

	return VisitTabStacks(root, [&panel](const DockTabStackPointer& stack) {
		const std::vector<DockPanelPointer>& panels = stack->Panels();
		return std::find(panels.begin(), panels.end(), panel) != panels.end();
	});
}

size_t Slate::CountPanels(const WidgetPointer& root) {
	size_t count = 0;
	VisitTabStacks(root, [&count](const DockTabStackPointer& stack) { count += stack->Count(); return false; });
	return count;
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

	return MakeSplitForZone(zone, incomingStack, targetRoot);
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

Slate::DockZone Slate::ResolveDockZone(const DockTarget& target, Vector2f point) {
	const Geometry& area = target.area;

	if (area.localSize.x <= 0.0f || area.localSize.y <= 0.0f) {
		return DockZone::None;
	}

	if (!area.ContainsAbsolute(point)) {
		return DockZone::None; //!< 領域の外ではdropしない.
	}

	//!< 1) マーカーの上に居るならそれを最優先する. 意図が明確なため.
	//!<    note: 当たり判定は HitTestDockZoneMarkers 側で見た目より広げている.
	if (const DockZone zone = HitTestDockZoneMarkers(area, point); zone != DockZone::None) {
		return zone;
	}

	//!< 2) tab barの行に居るならtabとして追加する. (中央と同じ)
	//!<    「ドッキング済みのtabへ足す」がこのgestureで済むようにする.
	if (target.stack != nullptr) {

		const float tabBarBottom = area.absolutePosition.y + DockTabStack::TabBarHeight();

		if (point.y < tabBarBottom) {
			return DockZone::Center;
		}
	}

	//!< 3) マーカーを外していても, 領域内の位置からゾーンを推定する.
	//!<    マーカーだけを条件にすると狙いが厳しすぎて実用にならない.
	//!<    端から kDockEdgeRatio までが上下左右, それより内側は中央.
	//!<    note: 最も近い縁を採用する. 角では距離が小さい方が勝つ.
	const float left   = (point.x - area.absolutePosition.x) / area.localSize.x;
	const float top    = (point.y - area.absolutePosition.y) / area.localSize.y;
	const float right  = 1.0f - left;
	const float bottom = 1.0f - top;

	const float nearest = std::min(std::min(left, right), std::min(top, bottom));

	if (nearest >= kDockEdgeRatio) {
		return DockZone::Center; //!< 中央寄りはtabとして追加.
	}

	if (nearest == left)  { return DockZone::Left; }
	if (nearest == right) { return DockZone::Right; }
	if (nearest == top)   { return DockZone::Top; }

	return DockZone::Bottom;
}

void Slate::PaintDockOverlay(ImGuiRenderer& renderer, const Geometry& dockArea, const DockTarget& preview) {
	const Style& style = Style::GetStyle();

	const Geometry target = preview.IsValid() ? preview.area : dockArea;

	//!< 1) 対象のtab stack全体をうっすら示す. どのpanelへ入るのかが分かるようにする.
	//!<    マーカーの上に居ないときも出すことで, drop先が決まっていないことも伝わる.
	renderer.DrawRect(target, style.colors.activeSoft);
	renderer.DrawBorder(target, style.colors.active, style.metrics.borderThin);

	//!< 2) drop結果の領域.
	if (preview.zone != DockZone::None) {
		const Geometry result
			= DockZonePreviewGeometry(target, preview.zone);

		renderer.DrawRect(result, style.colors.activeSoft);
		renderer.DrawBorder(result, style.colors.active, style.metrics.borderThick);

		//!< 中央(tabとして追加)のときはtab barを強調する.
		//!< 領域全体が同じ塗りになるため, 上下左右との区別が付かないのを避ける.
		if (preview.zone == DockZone::Center) {

			const Geometry tabBar = {
				target.absolutePosition,
				{ target.localSize.x, DockTabStack::TabBarHeight() },
				target.scale
			};

			renderer.DrawRect(tabBar, style.colors.active);
		}
	}

	//!< 3) 5つのゾーンマーカー. 描画と当たり判定で同じ BuildDockZoneMarkers を使う.
	DockZoneMarker markers[kDockZoneMarkerCount] = {};
	BuildDockZoneMarkers(target, markers);

	for (const DockZoneMarker& marker : markers) {

		const bool isHovered = (marker.zone == preview.zone);

		renderer.DrawRoundedRect(
			marker.box, isHovered ? style.colors.active : style.colors.panel, style.metrics.tabRounding
		);
		renderer.DrawBorder(
			marker.box, isHovered ? style.colors.text : style.colors.border, style.metrics.borderThin
		);

		//!< マーカーの中に結合結果を示す図を描く. これが無いとどのゾーンか判別できない.
		//!< 中央 = tabとして追加(枠のみ) / 上下左右 = 分割される側を塗る.
		//!< note: 移植元は colors.white を使っていたが engine には無いため colors.text で代用する.
		const Color4f ink = isHovered ? style.colors.text : style.colors.textDim;

		const float padding = marker.box.localSize.x * 0.22f;

		const Geometry inner = {
			{ marker.box.absolutePosition.x + padding, marker.box.absolutePosition.y + padding },
			{ marker.box.localSize.x - padding * 2.0f, marker.box.localSize.y - padding * 2.0f },
			target.scale
		};

		renderer.DrawBorder(inner, ink, style.metrics.borderThin);

		if (marker.zone != DockZone::Center) {
			renderer.DrawRect(DockZonePreviewGeometry(inner, marker.zone), ink);
		}
	}
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

	auto split = MakeSplitForZone(target.zone, incomingStack, target.stack);

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

		return MakeSplitForZone(target.zone, incoming, root);
	}

	//!< 方向 = 対象スタックの位置を「対象 + incoming」の分割に差し替える.
	//!< incoming の内部構造(分割・タブ)はそのまま維持される.
	auto split = MakeSplitForZone(target.zone, incoming, target.stack);

	return ReplaceNode(root, target.stack, split);
}

////////////////////////////////////////////////////////////////////////////////////////////
// dock host methods
////////////////////////////////////////////////////////////////////////////////////////////

bool Slate::IsPointOnAnyTab(const WidgetPointer& root, Vector2f absolutePosition) {
	return VisitTabStacks(root, [&absolutePosition](const DockTabStackPointer& stack) { return stack->ContainsTabAt(absolutePosition); });
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
	VisitTabStacks(root, [](const DockTabStackPointer& stack) { stack->SetTabBarRightInset(0.0f); return false; });
}

void Slate::ApplyDockingHost(const WidgetPointer& root, IDockingHost* host) {
	VisitTabStacks(root, [host](const DockTabStackPointer& stack) { stack->SetHost(host); return false; });
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
	if (panel == nullptr) {
		return false;
	}
	return VisitTabStacks(root, [&panel](const DockTabStackPointer& stack) { return stack->RemovePanel(panel); });
}
