#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "SlateDockPanel.h"

//* c++
#include <cstdint>
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
	// DockTabStack class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 複数の DockPanel をタブとして重ねて保持するノード.
	//!        タブバー + アクティブなパネルの中身 を描画する.
	class DockTabStack final
		: public Widget, public IDockNode {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* static parameter *//

		//!< スタイルは実行時に変更できるので, 都度参照する(定数にしない).
		static float TabBarHeight() { return Style::GetStyle().metrics.tabBarHeight; }

		static float TabPaddingX() { return Style::GetStyle().metrics.tabPaddingX; }

		//* panel option *//

		//! @brief タブを追加する.
		//! @note activate = true なら追加したタブを前面にする.
		//!       実行時に追加したパネルは, 裏に隠れると「表示されない」ように見えるため.
		void AddPanel(DockPanelPointer panel, bool activate = false);

		void AddPanels(const std::vector<DockPanelPointer>& panels);

		size_t Count() const { return panels_.size(); }

		bool IsEmpty() const { return panels_.empty(); }

		const std::vector<DockPanelPointer>& Panels() const { return panels_; }

		//! @brief パネルを取り除く(切り離し時に使用).
		bool RemovePanel(const DockPanelPointer& panel);

		//! @brief 切り離し要求の通知先(ウィンドウ側が設定する).
		void SetHost(IDockingHost* host) { host_ = host; }

		//! @brief 設定されている通知先を返す(未設定なら nullptr).
		//! @note ApplyDockingHost が分割ツリーの全スタックへ配れたかを確認するために使う.
		IDockingHost* GetHost() const { return host_; }

		//! @brief タブバー右端に空ける余白.
		//! @note フローティングウィンドウではタブ行にウィンドウ操作ボタンが重なるため,
		//!       タブがその下に潜り込まないように確保する.
		void SetTabBarRightInset(float inset) { tabBarRightInset_ = inset; }

		//! @brief 直近の描画でタブが占めた幅(ウィンドウのドラッグ判定に使う).
		float GetTabsWidth() const { return tabsWidth_; }

		//! @brief 絶対座標がいずれかのタブの上か.
		//! @note 分割レイアウトではタブ行がウィンドウ左上とは限らない.
		//!       直近の描画で確定した矩形(絶対座標)で判定するので, どんな入れ子でも正しく答えられる.
		bool ContainsTabAt(Vector2f absolutePosition) const;

		void CollectPanels(std::vector<DockPanelPointer>& out) const override { out.insert(out.end(), panels_.begin(), panels_.end()); }

		//! @brief レイアウト復元時にアクティブタブを戻すために使う.
		void SetActiveIndex(int32_t index);

		int32_t GetActiveIndex() const { return active_; }

		DockPanelPointer ActivePanel() const;

		//* widget option *//

		Vector2f ComputeDesiredSize(float scale, ImGuiRenderer* renderer) const override;

		void OnArrangeChildren(const Geometry& allotted, ArrangedChildren& out) const override;

		int32_t OnPaint(const PaintArguments& arguments, const Geometry& geometry, ImGuiRenderer* renderer, int32_t layer) const override;

		//* pointer option *//

		//!< タブを掴んでタブバーの外へ引き出すと, 新しいウィンドウとして切り離す.
		Reply OnMouseButtonDown(const Geometry& geometry, const PointerEvent& event) override;

		Reply OnMouseButtonUp(const Geometry& geometry, const PointerEvent& event) override;

		Reply OnMouseMove(const Geometry& geometry, const PointerEvent& event) override;

		void OnMouseLeave() override { hoverIndex_ = -1; lastMousePos_ = { -1.0f, -1.0f }; }

		Cursor GetCursor(const Geometry& /*geometry*/, Vector2f absolutePosition) const override { return (CloseButtonAt(absolutePosition) >= 0) ? Cursor::Hand : Cursor::Default; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		static constexpr float kTabMinWidth       = 72.0f; //!< タブ幅の下限.
		static constexpr float kTabMinShrunkWidth = 28.0f; //!< 幅が足りないときに縮めてでも表示する下限.

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		static constexpr float kTearOffThreshold = 16.0f; //!< タブバーからこの距離だけ離れたら切り離す.

		std::vector<DockPanelPointer> panels_;
		int32_t active_    = -1;
		int32_t dragIndex_ = -1; //!< ドラッグ中のタブ(-1 = なし).

		mutable int32_t hoverIndex_ = -1; //!< ホバー中のタブ.

		IDockingHost* host_     = nullptr; //!< 切り離し / 閉じる要求の通知先.
		float tabBarRightInset_ = 0.0f;    //!< 右端に空ける幅.

		mutable float tabsWidth_ = 0.0f; //!< 直近のタブ占有幅.
		mutable Vector2f lastMousePos_{ -1.0f, -1.0f };

		mutable std::vector<Geometry> tabRects_;   //!< 直近の描画で確定したタブ矩形.
		mutable std::vector<Geometry> closeRects_; //!< 同じく × ボタンの矩形.

		//=========================================================================================
		// private methods
		//=========================================================================================

		//! @brief 表示中のタブをフォーカスにする(種別色を入れる対象).
		void FocusActivePanel();

		//! @brief タブ1枚の描画(UE5 の Major Tab の見た目).
		void PaintTab(ImGuiRenderer* renderer, const Geometry& tab, const DockPanel& panel, bool isActive, bool isFocused, bool showClose) const;

		//! @brief 座標がどのタブの × 上か(-1 = どれでもない).
		int32_t CloseButtonAt(Vector2f position) const;

	};

	using DockTabStackPointer = std::shared_ptr<DockTabStack>;

}

SXAVENGER_ENGINE_NAMESPACE_END
