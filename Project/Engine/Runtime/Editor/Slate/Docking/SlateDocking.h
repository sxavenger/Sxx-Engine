#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* slate [docking]
#include "SlateDockTabStack.h"
#include "SlateSplitter.h"

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
	// DockZone enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ドロップゾーン.
	//!        Left/Right/Top/Bottom はその方向に分割して結合, Center はタブとして結合する.
	enum class DockZone : uint8_t {
		None,
		Left,
		Right,
		Top,
		Bottom,
		Center
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// dock zone marker constant
	////////////////////////////////////////////////////////////////////////////////////////////

	constexpr float kDockZoneMarkerSize    = 44.0f; //!< マーカー1つの一辺.
	constexpr float kDockZoneMarkerGap     = 8.0f;  //!< マーカー間の隙間.
	constexpr int32_t kDockZoneMarkerCount = 5;

	////////////////////////////////////////////////////////////////////////////////////////////
	// DockZoneMarker structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct DockZoneMarker {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		DockZone zone = DockZone::None;
		Geometry box{};

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// dock zone methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief マーカーの配置を計算する.
	//! @note 【重要】描画と当たり判定は必ずこの関数を共有すること.
	//!       別々に位置を計算すると「アイコンの位置」と「実際に反応する位置」がずれてしまう.
	//!       領域が小さい場合はマーカーを縮めて収める.
	void BuildDockZoneMarkers(const Geometry& area, DockZoneMarker out[kDockZoneMarkerCount]);

	//! @brief 絶対座標 point がどのマーカー上にあるかを返す.
	//! @note マーカー外なら None(= ドッキングしない). マーカーの見た目と判定が完全に一致する.
	DockZone HitTestDockZoneMarkers(const Geometry& area, Vector2f point);

	//! @brief ゾーンが指す結果領域(オーバーレイのハイライト表示用).
	Geometry DockZonePreviewGeometry(const Geometry& area, DockZone zone);

	////////////////////////////////////////////////////////////////////////////////////////////
	// dock tree methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief ツリーから最初に見つかった DockTabStack を返す(中央ドロップの結合先).
	DockTabStackPointer FindFirstTabStack(const WidgetPointer& root);

	//! @brief ツリー全体のパネルを回収する.
	std::vector<DockPanelPointer> CollectAllPanels(const WidgetPointer& root);

	//! @brief targetRoot に incoming(パネル群)をドッキングし, 新しいルートを返す.
	//!        Center は既存のタブスタックにタブとして追加, Left/Right は左右分割(Horizontal),
	//!        Top/Bottom は上下分割(Vertical)になる.
	WidgetPointer DockInto(WidgetPointer targetRoot, const std::vector<DockPanelPointer>& incoming, DockZone zone);

	//! @brief ツリー内の oldNode を newNode に差し替えた新しいルートを返す.
	//! @note DockIntoTarget から呼ばれている. 消さないこと.
	WidgetPointer ReplaceNode(const WidgetPointer& root, const WidgetPointer& oldNode, const WidgetPointer& newNode);

	////////////////////////////////////////////////////////////////////////////////////////////
	// DockTarget structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ドロップ先(カーソル下のタブスタックとその領域).
	struct DockTarget {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		bool IsValid() const { return stack != nullptr && zone != DockZone::None; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		DockTabStackPointer stack; //!< 結合先のタブスタック.
		Geometry area{};           //!< そのスタックが占める領域(絶対座標).
		DockZone zone = DockZone::None;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// dock target methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief ドックツリーを走査し, point を含むタブスタックとそのジオメトリを返す.
	//! @note ウィンドウ全体ではなく「タブスタック単位」でドッキングするために使う.
	bool FindDockTargetAt(const WidgetPointer& node, const Geometry& geometry, Vector2f point, DockTarget& out);

	//! @brief 指定したタブスタックを基準にドッキングする(タブ単位のドッキング).
	//!        Center は既存スタックへタブ追加, Left/Right/Top/Bottom は対象スタックの位置を
	//!        新しいスプリッタで置き換える.
	WidgetPointer DockIntoTarget(WidgetPointer root, const DockTarget& target, const std::vector<DockPanelPointer>& incoming);

	//! @brief 部分ツリーをそのままドッキングする(分割構造を保持する).
	//! @note Left/Right/Top/Bottom は incoming を「そのまま」相手側に置いて分割する.
	//!       Center はタブとして結合する(タブは分割を持てないので incoming のパネルを平坦化して束ねる).
	WidgetPointer DockSubtreeIntoTarget(WidgetPointer root, const DockTarget& target, WidgetPointer incoming);

	////////////////////////////////////////////////////////////////////////////////////////////
	// dock host methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief ツリー内のどれかのタブの上に絶対座標があるか.
	//! @note 分割レイアウトのウィンドウでも「タブを掴んだのか, 空きを掴んで移動したいのか」を判定できる.
	bool IsPointOnAnyTab(const WidgetPointer& root, Vector2f absolutePosition);

	//! @brief ウィンドウ右上を占めるタブスタックを返す.
	//! @note ウィンドウ操作ボタンはそこへ重なるので, そのスタックにだけ右余白を入れる.
	//!       横分割なら第2子, 縦分割なら第1子が右上側.
	DockTabStackPointer FindTopRightTabStack(const WidgetPointer& root);

	//! @brief ツリー内の全タブスタックのタブバー右余白をリセットする.
	//! @note 右余白はフローティングウィンドウのウィンドウ操作ボタンを避けるためのもので,
	//!       そのウィンドウのルートスタックにだけ意味がある. ドッキングでツリーの一部になった
	//!       後も余白が残ると, 幅の狭い列でタブが表示されなくなる(= タブが消えたように見える).
	//!       ツリーを組み替えるたびに必ずリセットすること.
	void ResetTabBarInsets(const WidgetPointer& root);

	//! @brief ツリー内の全タブスタックに切り離し要求の通知先を設定する.
	void ApplyDockingHost(const WidgetPointer& root, IDockingHost* host);

	//! @brief 空になったノードを取り除いてツリーを詰める.
	//!        空のタブスタックは削除(nullptr), 子が1つになったスプリッタはその子で置き換える.
	//! @note 新しいルートを返す(全て空なら nullptr).
	WidgetPointer PruneEmptyNodes(const WidgetPointer& root);

	//! @brief ツリーから特定のパネルを取り除く. 取り除けたら true.
	bool RemovePanelFromTree(const WidgetPointer& root, const DockPanelPointer& panel);

}

SXAVENGER_ENGINE_NAMESPACE_END
