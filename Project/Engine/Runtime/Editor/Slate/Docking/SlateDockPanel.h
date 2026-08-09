#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate
#include "../SlateIcon.h"

//* slate [style]
#include "../Style/SlateStyle.h"

//* slate [widgets]
#include "../Widgets/SlateCompoundWidget.h"

//* c++
#include <memory>
#include <string>
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
	// DockPanel class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ドッキング可能な最小単位. タイトルと中身を持つ.
	//! @note UE Slate の「タブ(SDockTab)」に相当する.
	class DockPanel final
		: public CompoundWidget {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* dock panel option *//

		//! @brief tabに表示する名前を設定する.
		//! @note UTF-8として扱う. iconは文字列の一部として自由な位置に入れられる.
		//!       ex. Title(std::format("{} Viewport", Slate::Icon::Videocam))
		std::shared_ptr<DockPanel> Title(std::string_view title) { title_ = title; return SharedThis(); }

		std::shared_ptr<DockPanel> Content(WidgetPointer widget) { SetPointer(std::move(widget)); return SharedThis(); }

		//! @brief 種別ごとのタブ色. UE5 のタブは種別ごとに色が付く(Viewport / Details / Content ...).
		//!        「アクティブ かつ フォーカス中」のタブの上端に引く色帯に使う.
		std::shared_ptr<DockPanel> TabColor(Color4f color) { tabColor_ = color; return SharedThis(); }

		//! @brief 閉じるボタンを出すか(UE5 では固定タブは閉じられない).
		std::shared_ptr<DockPanel> Closable(bool closable) { closable_ = closable; return SharedThis(); }

		//* getter *//

		const std::string& GetTitle() const { return title_; }

		Color4f GetTabColor() const { return tabColor_; }

		bool IsClosable() const { return closable_; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::string title_ = "Panel";
		Color4f tabColor_  = Style::GetStyle().colors.active;
		bool closable_     = true;

		//=========================================================================================
		// private methods
		//=========================================================================================

		//! @brief フルエントなセッタで自身を shared_ptr<DockPanel> として返すためのヘルパ.
		//! @note 移植元の SharedThis(this) に相当する. Engine 側には同等の共通ヘルパが無いため,
		//!       DockPanel 内に static_pointer_cast でくるむだけの private メソッドとして持つ.
		std::shared_ptr<DockPanel> SharedThis() { return std::static_pointer_cast<DockPanel>(shared_from_this()); }

	};

	using DockPanelPointer = std::shared_ptr<DockPanel>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// dock panel focus methods
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief フォーカス中のパネル(アプリ全体で常に1枚まで).
	//! @note 分割レイアウトではどのタブスタックにも「アクティブなタブ」があるので,
	//!       アクティブかどうかだけで種別色を入れると, 画面中の全スタックが色付いて
	//!       「今どこを触っているか」が分からなくなる. UE5 と同じく, 直前に操作した
	//!       タブ1枚にだけ種別色を入れるため, ここでフォーカスを1つ持つ.
	//! @note weak_ptr で持つのは, ドッキングやレイアウト復元でパネルが破棄されても
	//!       宙に浮いたポインタを残さないため(期限切れ = フォーカス無し).
	inline std::weak_ptr<DockPanel>& FocusedPanelRef() {
		//!< inline 関数のローカル static なので, 全翻訳単位で実体は1つ.
		static std::weak_ptr<DockPanel> focused;
		return focused;
	}

	inline void SetFocusedPanel(const DockPanelPointer& panel) { FocusedPanelRef() = panel; }

	inline DockPanelPointer GetFocusedPanel() { return FocusedPanelRef().lock(); }

	inline bool IsPanelFocused(const DockPanel& panel) {
		const DockPanelPointer focused = GetFocusedPanel();
		return focused && focused.get() == &panel;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// IDockNode class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ドックツリーのノード共通インターフェイス.
	//!        ツリーからパネルを回収する(ウィンドウ統合時に使用).
	class IDockNode {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		virtual ~IDockNode() noexcept = default;

		virtual void CollectPanels(std::vector<DockPanelPointer>& out) const = 0;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// IDockingHost class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ドックツリーを載せている側(= ネイティブウィンドウ)への通知窓口.
	//!        タブがタブバーの外へドラッグされたときに, 新しいウィンドウとして
	//!        切り離す(tear-off)ことを要求する.
	//! @note 実際の処理は WndProc の中ではなくメインループで行う(再入を避けるため).
	class IDockingHost {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		virtual ~IDockingHost() noexcept = default;

		//! @brief このパネルを切り離してよいか.
		//! @note 切り離すとウィンドウが空になり閉じられてしまう場合(= フローティング
		//!       ウィンドウにタブが1枚だけ)は false. その状態で切り離すと, 元ウィンドウが
		//!       閉じる途中に同じウィンドウへ戻す操作ができてしまい, タブごと破棄されて消失する.
		//!       UE5 も同様に単独タブは引き出せない.
		virtual bool CanTearOff(const DockPanelPointer& panel) const = 0;

		virtual void RequestTearOff(DockPanelPointer panel, Vector2f screenPosition) = 0;

		//! @brief タブの × が押された. ツリーから取り除き, 空になったノードを詰める.
		virtual void RequestPanelClose(DockPanelPointer panel) = 0;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
