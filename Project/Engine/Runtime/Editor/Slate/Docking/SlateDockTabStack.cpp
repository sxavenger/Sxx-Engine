#include "SlateDockTabStack.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
// DockTabStack class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Slate::DockTabStack::AddPanel(DockPanelPointer panel, bool activate) {
	if (!panel) {
		return;
	}

	panels_.push_back(std::move(panel));

	if (active_ < 0 || activate) {
		active_ = static_cast<int32_t>(panels_.size()) - 1;
	}

	//!< 起動直後にどのタブにも種別色が入らないのを避ける.
	//!< 誰もフォーカスされていなければ最初の1枚を採用する.
	if (!GetFocusedPanel()) {
		SetFocusedPanel(panels_.back());
	}
}

void Slate::DockTabStack::AddPanels(const std::vector<DockPanelPointer>& panels) {
	for (const auto& panel : panels) {
		AddPanel(panel);
	}
}

bool Slate::DockTabStack::RemovePanel(const DockPanelPointer& panel) {
	for (size_t i = 0; i < panels_.size(); ++i) {
		if (panels_[i] != panel) {
			continue;
		}

		panels_.erase(panels_.begin() + static_cast<ptrdiff_t>(i));

		if (panels_.empty()) {
			active_ = -1;

		} else if (active_ >= static_cast<int32_t>(panels_.size())) {
			active_ = static_cast<int32_t>(panels_.size()) - 1;
		}

		tabRects_.clear();
		return true;
	}

	return false;
}

bool Slate::DockTabStack::ContainsTabAt(Vector2f absolutePosition) const {
	for (const Geometry& tab : tabRects_) {
		if (tab.localSize.x <= 0.0f) {
			continue;
		}

		if (tab.ContainsAbsolute(absolutePosition)) {
			return true;
		}
	}

	return false;
}

void Slate::DockTabStack::SetActiveIndex(int32_t index) {
	if (index >= 0 && index < static_cast<int32_t>(panels_.size())) {
		active_ = index;
	}
}

Slate::DockPanelPointer Slate::DockTabStack::ActivePanel() const {
	if (active_ < 0 || active_ >= static_cast<int32_t>(panels_.size())) {
		return nullptr;
	}

	return panels_[static_cast<size_t>(active_)];
}

Vector2f Slate::DockTabStack::ComputeDesiredSize(float scale, ImGuiRenderer* renderer) const {
	Vector2f content{};

	if (auto panel = ActivePanel()) {
		content = panel->ComputeDesiredSize(scale, renderer);
		panel->CacheDesiredSize(content);
	}

	return { content.x, content.y + TabBarHeight() };
}

void Slate::DockTabStack::OnArrangeChildren(const Geometry& allotted, ArrangedChildren& out) const {
	auto panel = ActivePanel();

	if (!panel) {
		return;
	}

	const Vector2f offset{ 0.0f, TabBarHeight() };
	const Vector2f size{ allotted.localSize.x, allotted.localSize.y - TabBarHeight() };

	if (size.y <= 0.0f) {
		return;
	}

	out.push_back({ panel, allotted.MakeChild(offset, size) });
}

int32_t Slate::DockTabStack::OnPaint(const PaintArguments& arguments, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const {

	//!< パネル本体の地色(UE5 はタブより明るい面).
	renderer->DrawRect(geometry, Style::GetStyle().colors.panel);

	//!< タブバー背景.
	Geometry barGeometry = geometry;
	barGeometry.localSize.y = TabBarHeight();
	renderer->DrawRect(barGeometry, Style::GetStyle().colors.tabBar);

	//!< タブを左から並べる(矩形をキャッシュしてヒットテストに使う).
	tabRects_.clear();
	closeRects_.clear();

	//!< ウィンドウ操作ボタンが重なる分は使わない.
	const float usableRight = geometry.absolutePosition.x + geometry.localSize.x - tabBarRightInset_;
	float x = geometry.absolutePosition.x + Style::GetStyle().metrics.tabGap;

	for (size_t i = 0; i < panels_.size(); ++i) {
		const auto& panel    = panels_[i];
		const bool isActive  = (static_cast<int32_t>(i) == active_);
		const bool isHover   = (static_cast<int32_t>(i) == hoverIndex_);

		//!< iconはtitleに含まれるため, 実寸の計測だけで幅が決まる.
		const Vector2f textSize = renderer->MeasureTextA(panel->GetTitle(), Style::GetStyle().metrics.fontBody);

		//!< 幅 = 余白 + 文字 + 余白 + ×(あるときだけ).
		const bool showClose = panel->IsClosable() && (isActive || isHover);
		float w = TabPaddingX() * 2.0f + textSize.x;

		if (panel->IsClosable()) {
			w += Style::GetStyle().metrics.paddingM + Style::GetStyle().metrics.tabCloseSize;
		}
		if (w < kTabMinWidth) {
			w = kTabMinWidth;
		}

		//!< 幅が足りない場合は「消さずに詰める」.
		//!< ここでスキップするとタブが画面から消えて操作できなくなる.
		if (x + w > usableRight) {
			const float remain = usableRight - x;

			if (remain < kTabMinShrunkWidth) {
				//!< 表示できる余地が無い. 以降のタブも同様なので打ち切る.
				//!< (添字を保つため空矩形を積む)
				for (size_t k = i; k < panels_.size(); ++k) {
					tabRects_.push_back(Geometry{});
					closeRects_.push_back(Geometry{});
				}
				break;
			}

			w = remain; //!< 収まる幅に縮めて描く.
		}

		const Geometry tab{ { x, geometry.absolutePosition.y }, { w, TabBarHeight() }, geometry.scale };
		tabRects_.push_back(tab);

		PaintTab(renderer, tab, *panel, isActive, IsPanelFocused(*panel), showClose);
		x += w + Style::GetStyle().metrics.tabGap;
	}

	tabsWidth_ = x - geometry.absolutePosition.x; //!< タブが占めた幅.

	//!< タブバーとパネルの境界. アクティブタブの区間だけ線を切って
	//!< タブとパネル本体が地続きに見えるようにする(UE5 と同じ表現).
	const float underline = Style::GetStyle().metrics.tabUnderlineSize;

	if (underline > 0.0f) {
		const float by = geometry.absolutePosition.y + TabBarHeight() - underline * 0.5f;
		renderer->DrawLine(
			{ geometry.absolutePosition.x, by },
			{ geometry.absolutePosition.x + geometry.localSize.x, by },
			Style::GetStyle().colors.tabUnderline, underline);

		//!< 切り欠き. 装飾ではなく上の線の消しゴムなので,
		//!< 色はアクティブタブの背景と必ず同じにする(太さも揃える).
		if (active_ >= 0 && active_ < static_cast<int32_t>(tabRects_.size())) {
			const Geometry& activeTab = tabRects_[static_cast<size_t>(active_)];
			renderer->DrawLine(
				{ activeTab.absolutePosition.x + 1.0f, by },
				{ activeTab.absolutePosition.x + activeTab.localSize.x - 1.0f, by },
				Style::GetStyle().colors.tabActive, underline);
		}
	}

	//!< アクティブなパネルの中身.
	int32_t maxLayer = layer;
	ArrangedChildren arranged;
	OnArrangeChildren(geometry, arranged);

	for (const auto& child : arranged) {
		const int32_t childLayer = child.widget->OnPaint(arguments, child.geometry, renderer, layer + 1);
		maxLayer = std::max(maxLayer, childLayer);
	}

	return maxLayer;
}

Slate::Reply Slate::DockTabStack::OnMouseButtonDown(const Geometry& geometry, const PointerEvent& event) {
	if (event.button != PointerEvent::Button::Left) {
		return Reply::Unhandled();
	}

	//!< × が押されたらタブを閉じる(ドラッグ開始より優先).
	const int32_t closeIndex = CloseButtonAt(event.screenPosition);
	if (closeIndex >= 0 && closeIndex < static_cast<int32_t>(panels_.size())) {
		if (host_) {
			host_->RequestPanelClose(panels_[static_cast<size_t>(closeIndex)]);
		}
		return Reply::Handled();
	}

	for (size_t i = 0; i < tabRects_.size(); ++i) {
		if (!tabRects_[i].ContainsAbsolute(event.screenPosition)) {
			continue;
		}

		active_    = static_cast<int32_t>(i);
		dragIndex_ = static_cast<int32_t>(i);
		FocusActivePanel(); //!< タブを直接クリックした.

		//!< 掴んだ状態を保持するためマウスをキャプチャする.
		return Reply::Handled().CaptureMouse(shared_from_this());
	}

	//!< タブ以外 = パネルの中身が押された.
	//!< 中身は他ウィジェットが入力ハンドラを持たない限り, 押下は leaf から順にバブルして
	//!< ここへ来る. 触っているスタックが分かる唯一の場所なので, ここでフォーカスを移す.
	//!<
	//!< Unhandled を返すのは従来どおり. 仕切りのドラッグなど, 外側のウィジェットが
	//!< 受け取る余地を残す必要がある.
	FocusActivePanel();
	return Reply::Unhandled();
}

Slate::Reply Slate::DockTabStack::OnMouseMove(const Geometry& geometry, const PointerEvent& event) {
	lastMousePos_ = event.screenPosition; //!< × のホバー表示に使う.

	if (dragIndex_ < 0) {
		//!< ホバー中のタブを記録(ハイライト表示用).
		hoverIndex_ = -1;

		for (size_t i = 0; i < tabRects_.size(); ++i) {
			if (tabRects_[i].ContainsAbsolute(event.screenPosition)) {
				hoverIndex_ = static_cast<int32_t>(i);
				break;
			}
		}

		return (hoverIndex_ >= 0) ? Reply::Handled() : Reply::Unhandled();
	}

	if (dragIndex_ >= static_cast<int32_t>(panels_.size())) {
		dragIndex_ = -1;
		return Reply::Handled().ReleaseMouseCapture();
	}

	//!< タブバーの矩形から十分に離れたら切り離しとみなす.
	const float top    = geometry.absolutePosition.y - kTearOffThreshold;
	const float bottom = geometry.absolutePosition.y + TabBarHeight() + kTearOffThreshold;
	const bool outside = (event.screenPosition.y < top) || (event.screenPosition.y > bottom);

	if (!outside) {
		return Reply::Handled();
	}

	DockPanelPointer panel = panels_[static_cast<size_t>(dragIndex_)];

	//!< 切り離すとウィンドウが空になって閉じられてしまう場合は引き出さない.
	//!< (タブ1枚のフローティングウィンドウ. ウィンドウごと動かせばよい)
	if (host_ && !host_->CanTearOff(panel)) {
		return Reply::Handled(); //!< ドラッグ状態は維持し, 何もしない.
	}

	dragIndex_ = -1;
	//!< 実際のウィンドウ生成はメインループで行う(WndProc 内の再入を避けるため).
	if (host_) {
		host_->RequestTearOff(std::move(panel), event.screenPosition);
	}
	return Reply::Handled().ReleaseMouseCapture();
}

Slate::Reply Slate::DockTabStack::OnMouseButtonUp(const Geometry& geometry, const PointerEvent& event) {
	if (dragIndex_ < 0) {
		return Reply::Unhandled();
	}

	dragIndex_ = -1;
	return Reply::Handled().ReleaseMouseCapture();
}

void Slate::DockTabStack::FocusActivePanel() {
	if (auto panel = ActivePanel()) {
		SetFocusedPanel(panel);
	}
}

void Slate::DockTabStack::PaintTab(ImGuiRenderer* renderer, const Geometry& tab, const DockPanel& panel, bool isActive, bool isFocused, bool showClose) const {
	//!< 色を変えるのは選択中のタブだけ. ホバーでは色を変えない.
	const Color4f bg = isActive ? Style::GetStyle().colors.tabActive : Style::GetStyle().colors.tabBar;
	//!< 上端だけ丸める(下はパネルと繋がるので角を残す).
	renderer->DrawRoundedRect(tab, bg, Style::GetStyle().metrics.tabRounding, CornerFlags::Top);

	//!< 種別色の帯は, 直前に操作したタブ1枚にだけ入れる.
	//!< 分割していると全スタックにアクティブタブがあるので,
	//!< アクティブなだけで色を入れると画面中が色付いてしまう.
	if (isActive && isFocused) {
		renderer->DrawRoundedRect(
			Geometry{ tab.absolutePosition, { tab.localSize.x, Style::GetStyle().metrics.tabAccentH }, tab.scale },
			panel.GetTabColor(), Style::GetStyle().metrics.tabRounding, CornerFlags::Top);
	}

	const float cy = tab.absolutePosition.y + TabBarHeight() * 0.5f;
	float cursorX  = tab.absolutePosition.x + TabPaddingX();
	//!< 選択中だけ明るくする. アイコンも文字と同じ色で揃える.
	//!< Engine の Style::Colors には白色(移植元の colors.white)が無いため, 代わりに
	//!< 主要な文字色である colors.text を使う(TitleBar の PaintGlyph と同じ扱い).
	const Color4f ink = isActive ? Style::GetStyle().colors.text : Style::GetStyle().colors.textDim;

	//!< タブ名. iconはtitleに含まれる文字として一緒に描かれる.
	//!< note: iconのfontは本文と同じatlasへmergeされているため, 1回のDrawTextAで混在して描ける.
	const Vector2f textSize = renderer->MeasureTextA(panel.GetTitle(), Style::GetStyle().metrics.fontBody);
	renderer->DrawTextA({ cursorX, cy - textSize.y * 0.5f }, panel.GetTitle(), ink, Style::GetStyle().metrics.fontBody);

	//!< × ボタン(右端).
	Geometry closeRect{};

	if (showClose) {
		const float cs = Style::GetStyle().metrics.tabCloseSize;
		closeRect = Geometry{
			{ tab.absolutePosition.x + tab.localSize.x - TabPaddingX() - cs, cy - cs * 0.5f },
			{ cs, cs }, tab.scale };

		const bool closeHover = closeRect.ContainsAbsolute(lastMousePos_);
		if (closeHover) {
			renderer->DrawRoundedRect(closeRect, Style::GetStyle().colors.close, 2.0f);
		}

		const float pad = 3.0f;
		const Color4f closeInk = closeHover ? Style::GetStyle().colors.text : Style::GetStyle().colors.iconDim;
		renderer->DrawLine(
			{ closeRect.absolutePosition.x + pad, closeRect.absolutePosition.y + pad },
			{ closeRect.absolutePosition.x + cs - pad, closeRect.absolutePosition.y + cs - pad },
			closeInk, Style::GetStyle().metrics.borderThin);
		renderer->DrawLine(
			{ closeRect.absolutePosition.x + pad, closeRect.absolutePosition.y + cs - pad },
			{ closeRect.absolutePosition.x + cs - pad, closeRect.absolutePosition.y + pad },
			closeInk, Style::GetStyle().metrics.borderThin);
	}

	closeRects_.push_back(closeRect);
}

int32_t Slate::DockTabStack::CloseButtonAt(Vector2f position) const {
	for (size_t i = 0; i < closeRects_.size(); ++i) {
		if (closeRects_[i].localSize.x <= 0.0f) {
			continue;
		}

		if (closeRects_[i].ContainsAbsolute(position)) {
			return static_cast<int32_t>(i);
		}
	}

	return -1;
}
