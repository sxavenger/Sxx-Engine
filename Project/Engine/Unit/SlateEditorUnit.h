#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Framework/Core/IUnit.h>

//* engine [core]
#include <Runtime/Core/Configuration/Configuration.h>

//* engine [platform]
#include <Runtime/Platform/Window.h>

//* engine [application]
#include <Runtime/Application/Viewport.h>

//* engine [editor]
#include <Runtime/Editor/Slate/SlateApplication.h>
#include <Runtime/Editor/Slate/Renderer/SlateImGuiRenderer.h>
#include <Runtime/Editor/Slate/Widgets/SlateWindow.h>
#include <Runtime/Editor/Slate/Docking/SlateDocking.h>

//* lib
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Time/RunTimeTracker.h>
#include <Lib/Math/Vector2.h>

//* c++
#include <cstdint>
#include <cstddef>
#include <list>
#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Slate editorの起動 / 更新 / 描画を行うUnit.
//! @note native windowはdockingのtear-off / mergeで増減するため, このUnitが所有して管理する.
//!       WindowUnitには依存しない. (両方を登録するとmessage pumpが二重に回るため併用しない.)
class SlateEditorUnit final
	: public Framework::IUnit {
public:

	////////////////////////////////////////////////////////////////////////////////////////
	// EditorWindow structure
	////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 1つのnative windowに対応する編集単位.
	//! @note ImGuiのcontextはnative windowごとに1つ必要なため, rendererもwindowごとに持つ.
	//!       hover / mouse captureの状態もwindowごとに独立するため, applicationも1つずつ持つ.
	//! @note IDockingHostを実装し, tabの切り離し / tabを閉じる要求を受ける.
	//!       「どのwindowから引き出されたか」が要求と一緒に必要なため, hostはwindowごとに持つ.
	//! @note windows_(std::list)がunique_ptrで所有するため, このアドレスは要素の追加 / 削除で移動しない.
	//!       DockTabStackへ渡すhostのポインタと, 遅延要求が持つsourceのポインタはこの前提に依存する.
	//!       (破棄されるwindowを指す要求はCollectClosedWindowsで捨てる.)
	struct EditorWindow final
		: public Editor::Slate::IDockingHost {
	public:

		//======================================================================================
		// public methods
		//======================================================================================

		//* docking host methods *//

		//! @brief このpanelを切り離してよいか.
		//! @note treeのpanelが1枚だけのsub windowは不可. 切り離すとwindowが空になって閉じられ,
		//!       閉じる途中に同じwindowへ戻す操作ができてしまい, tabごと破棄されて消えるため.
		bool CanTearOff(const Editor::Slate::DockPanelPointer& panel) const override;

		//! @brief tabがtab barの外へ引き出された.
		//! @note DockTabStack::OnMouseMoveの中から呼ばれるため, 要求を積むだけに留める.
		//!       ここでwindowを生成したりtreeを差し替えると, 走査中のtreeを壊す.
		void RequestTearOff(Editor::Slate::DockPanelPointer panel, Vector2f screenPosition) override;

		//! @brief tabの × が押された.
		//! @note RequestTearOffと同じ理由で遅延させる. (OnMouseButtonDownの中から呼ばれる)
		void RequestPanelClose(Editor::Slate::DockPanelPointer panel) override;

		//======================================================================================
		// public variables
		//======================================================================================

		//* platform *//

		Application::Viewport viewport; //!< native windowとswap chain.

		bool isMain = false; //!< main windowかどうか. main windowを閉じるとapplicationが終了する.

		//* slate *//

		Editor::Slate::ImGuiRenderer renderer;    //!< このwindow専用のImGui context.
		Editor::Slate::Application   application; //!< layoutの駆動と入力routing.

		std::shared_ptr<Editor::Slate::Window> root = nullptr; //!< ルートwidget. (chromeを含む表示用のtree)

		//!< dockingのtreeだけを指す. SplitterとDockTabStackのみで構成される.
		//!< note: Docking::ApplyDockingHost / PruneEmptyNodes / FindDockTargetAt などのtraversalは
		//!<       DockTabStackとSplitterしか辿らない. rootにはTitleBarを載せるためVerticalBoxやOverlayが
		//!<       挟まっており, rootを渡すと1段目で打ち切られてhostが配られない. そのため分けて持つ.
		Editor::Slate::WidgetPointer dockRoot = nullptr;

		//* runtime parameter *//

		Vector2f mousePosition   = {}; //!< client座標系のマウス位置.
		bool     mouseButtons[3] = {}; //!< 前frameのマウスボタンの押下状態. (edge検出用)

		//* docking *//

		SlateEditorUnit* owner = nullptr; //!< 要求の実処理を行うUnit. (treeの差し替えはUnitの責務)

		//!< drag中に表示するdrop先. zone == Noneならマーカーのみを描く.
		//!< (drag中でなければlocalSizeが0のままなので何も描かれない.)
		Editor::Slate::DockTarget dockPreview = {};

	};

	using EditorWindowPointer = std::unique_ptr<EditorWindow>;

	////////////////////////////////////////////////////////////////////////////////////////
	// TearOffRequest structure
	////////////////////////////////////////////////////////////////////////////////////////
	//! @brief tabの切り離し要求.
	//! @note 入力処理の途中ではwindowの生成もtreeの差し替えもできないため, 一度積んでから処理する.
	struct TearOffRequest {
	public:

		//======================================================================================
		// public variables
		//======================================================================================

		EditorWindow* source = nullptr; //!< 引き出し元のwindow. 処理前に破棄され得るため生存を確認して使う.

		Editor::Slate::DockPanelPointer panel = nullptr; //!< 引き出すpanel.

		Vector2f screenPosition = {}; //!< 引き出した位置. (screen座標. 新しいwindowの配置に使う)

	};

	////////////////////////////////////////////////////////////////////////////////////////
	// PanelCloseRequest structure
	////////////////////////////////////////////////////////////////////////////////////////
	//! @brief tabを閉じる要求.
	struct PanelCloseRequest {
	public:

		//======================================================================================
		// public variables
		//======================================================================================

		EditorWindow* source = nullptr; //!< 閉じるtabを持つwindow.

		Editor::Slate::DockPanelPointer panel = nullptr; //!< 閉じるpanel.

	};

	////////////////////////////////////////////////////////////////////////////////////////
	// WindowDrag structure
	////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Unitが自前で行っているwindowの移動状態.
	//! @note OSのtitle barでの移動(HTCAPTION)はmodalなmove loopに入り, その間message pumpはOS側で
	//!       回るためUnitのUpdate / Renderが一切走らない. drop先のoverlayを出せずドッキングの判定も
	//!       できないため, tab barの空きを掴んだ場合はUnitがSetWindowPosでwindowを動かす.
	struct WindowDrag {
	public:

		//======================================================================================
		// public variables
		//======================================================================================

		EditorWindow* window = nullptr; //!< 移動中のwindow. (nullptr = 移動していない)

		Vector2i grabOffset = {}; //!< 掴んだ点のwindow矩形左上からのoffset. (screen座標)

	};

public:

	//======================================================================================
	// public methods
	//======================================================================================

	//* unit methods *//

	void Setup(Framework::Pipeline& pipeline) override;

	//* window option *//

	//! @brief editor windowを生成する.
	//! @note dockingのtear-offでsub windowを増やす場合もここを通す.
	//! @retval 生成したwindow. 生成に失敗した場合はnullptr.
	RefPtr<EditorWindow> CreateEditorWindow(const std::wstring& name, const Vector2ui& client, bool isMain = false);

	//! @brief editor windowの破棄を予約する.
	//! @note frameの途中でlistから消すとiteratorが壊れるため, EndFrameで回収する.
	void RequestCloseEditorWindow(RefPtr<EditorWindow> window);

	RefPtr<EditorWindow> GetMainEditorWindow() const;

	const std::list<EditorWindowPointer>& GetEditorWindows() const { return windows_; }

private:

	//======================================================================================
	// private variables
	//======================================================================================

	//* constant *//

	//!< pollingするマウスボタンの数. (左 / 右 / 中)
	static constexpr size_t kMouseButtonCount = 3;

	//!< main windowの既定のclient size.
	static constexpr Vector2ui kDefaultClientSize = { 1280, 720 };

	//!< 切り離しで生成するwindowの既定のclient size.
	static constexpr Vector2ui kTearOffClientSize = { 520, 360 };

	//!< 切り離したwindowをcursorのどれだけ左に置くか[px]. tab barを掴んだまま動かせるようにする.
	static constexpr int32_t kTearOffCursorOffsetX = 60;

	//* windows *//

	std::list<EditorWindowPointer> windows_;      //!< 所有しているeditor window.
	std::list<EditorWindow*>       closeRequests_; //!< 破棄が予約されたwindow.

	//* runtime parameter *//

	RunTimeTracker frameTracker_; //!< frame間の時間を計測するtracker.

	//!< 起動からの累計時間. Widget::PaintArguments::timeへ渡す. (deltaTimeを渡すとアニメーションが進まない.)
	TimePointf<TimeUnit::Second> elapsedTime_ = {};

	//* docking *//

	std::vector<TearOffRequest>    tearOffRequests_    = {}; //!< 積まれた切り離し要求.
	std::vector<PanelCloseRequest> panelCloseRequests_ = {}; //!< 積まれたtabを閉じる要求.

	WindowDrag windowDrag_ = {}; //!< Unitが移動させているwindow.

	//!< 前frameの左ボタンの押下状態. windowのdragを開始するedgeの検出に使う.
	//!< note: EditorWindow::mouseButtonsはUpdateInputが更新済みのため, edgeを取るには別に持つ必要がある.
	bool isPreviousLeftButtonDown_ = false;

	//!< 生成したwindowの累計数. window classの名前を一意にするために使う.
	uint32_t windowSerial_ = 0;

	//======================================================================================
	// private methods
	//======================================================================================

	//* unit helper methods *//

	void InitEditor();

	void UpdateEditor();

	void RenderEditor();

	void PresentEditor();

	void TermEditor();

	//! @brief loopを抜ける条件.
	bool IsRequestExit();

	//* chrome helper methods *//

	//! @brief OSのtitle barを消し, 自前のTitleBarで移動 / リサイズできるように配線する.
	void SetupWindowChrome(EditorWindow& window);

	//! @brief borderless windowのヒットテスト. (Platform::Windowから呼ばれる)
	Platform::Window::NonClientArea HitTestWindowChrome(const EditorWindow& window, const Vector2i& client) const;

	//! @brief borderless windowのカーソル形状. (Platform::Windowから呼ばれる)
	Platform::Window::CursorShape QueryWindowCursor(const EditorWindow& window) const;

	//! @brief TitleBarの最小化 / 最大化 / 閉じるボタンを配線したwidgetを返す.
	Editor::Slate::WidgetPointer CreateTitleBar(EditorWindow& window, bool isButtonsOnly);

	//! @brief dockRootをTitleBarで包み直してrootへ設定する.
	//! @note dockRootを差し替えたら必ず呼ぶ. 呼ばないと表示用のtreeが古いdockRootを指したままになる.
	void RebuildWindowChrome(EditorWindow& window);

	//* window helper methods *//

	//! @brief widget treeを構築する.
	void BuildLayout(EditorWindow& window);

	//! @brief 破棄が予約された / OSに閉じられたwindowを回収する.
	void CollectClosedWindows();

	//* docking helper methods *//

	//! @brief 切り離し要求を積む.
	//! @param clientPosition 引き出した位置. (PointerEventはclient座標系なのでscreen座標へ変換して持つ)
	void EnqueueTearOff(EditorWindow* source, Editor::Slate::DockPanelPointer panel, Vector2f clientPosition);

	//! @brief tabを閉じる要求を積む.
	void EnqueuePanelClose(EditorWindow* source, Editor::Slate::DockPanelPointer panel);

	//! @brief 積まれた要求をまとめて処理する.
	//! @note widget treeの差し替えとwindowの生成 / 破棄を伴うため, 必ず入力処理の外で呼ぶ.
	void ProcessPendingRequests();

	void ProcessTearOffRequest(const TearOffRequest& request);

	void ProcessPanelCloseRequest(const PanelCloseRequest& request);

	//! @brief windowを指す要求とdrag状態を捨てる.
	//! @note windowを破棄する前に呼ぶ. 残すとerase後にdangling pointerを触る.
	void DiscardRequests(const EditorWindow* window);

	//! @brief window内の全tab stackへhostを配り直す.
	//! @note 組み替えで生成されたDockTabStackにはhostが入っていないため, treeを触った直後に必ず呼ぶ.
	void ApplyDockingHostToWindow(EditorWindow& window);

	//! @brief windowの最初のtab stackへpanelを追加する. (tab stackが無ければ作る)
	void AddPanelToEditorWindow(EditorWindow& window, const Editor::Slate::DockPanelPointer& panel);

	//! @brief panelが1枚も無くなったwindowを閉じる. (main windowは空でも閉じない)
	void CloseIfEmptyEditorWindow(EditorWindow& window);

	//! @brief windows_が所有しているwindowか. (要求が持つポインタの検証)
	bool IsAliveEditorWindow(const EditorWindow* window) const;

	//* window drag helper methods *//

	//! @brief windowのdragを更新する. (移動 / drop先の判定 / dropの実行)
	void UpdateWindowDrag();

	//! @brief cursor下のwindowのtab barの空きを掴んだかを判定し, dragを開始する.
	bool TryBeginWindowDrag(const Vector2i& cursor);

	void BeginWindowDrag(EditorWindow& window, const Vector2i& cursor);

	//! @brief drop先をcursor位置から求め, 対象のwindowへ記録する. (他のwindowの記録は消す)
	void UpdateDockPreview(const Vector2i& cursor);

	//! @brief dragを終了し, drop先があればtreeをそのまま結合する.
	void EndWindowDrag(const Vector2i& cursor);

	void ClearDockPreviews();

	//! @brief windowをscreen座標へ移動させる.
	//! @note Viewport::Createに位置の指定が無いため, 生成後にここで移動させる.
	static void MoveEditorWindow(EditorWindow& window, const Vector2i& screenPosition);

	//! @brief cursorの下にあるdrop先のwindowを探す.
	//! @note マーカーの表示(UpdateDockPreview)とdropの判定(EndWindowDrag)は必ずこの関数を通す.
	//!       受け入れ可否までここで判定することで「マーカーは出るのに落とせない」ズレを防ぐ.
	EditorWindow* FindEditorWindowUnderCursor(const EditorWindow* dragged, const Vector2i& cursor) const;

	//! @brief draggedをtargetへドッキングしてよいか.
	static bool CanDockInto(const EditorWindow& dragged, const EditorWindow& target);

	//! @brief screen座標がwindowの矩形内か.
	static bool ContainsScreenPoint(const EditorWindow& window, const Vector2i& cursor);

	//* frame helper methods *//

	//! @brief 1つのwindowのlayoutと描画を行う.
	void RenderWindow(EditorWindow& window, TimePointf<TimeUnit::Second> deltaTime);

	//! @brief drop先のoverlay(対象のハイライトとゾーンのマーカー)を描く.
	//! @note ImGuiのwidgetより手前に出すため, DrawTarget::Foregroundへ描く.
	static void RenderDockOverlay(EditorWindow& window);

	//! @brief マウスをpollingし, ImGuiとSlateへイベントを配送する.
	void UpdateInput(EditorWindow& window);

	//* input helper methods *//

	//! @brief 仮想keyが押下されているかを返す.
	//! @note TODO: Platform::InputSystemが公開されたら, そちらのMouse / Keyboardへ移行する.
	static bool IsVirtualKeyDown(int32_t key);

};

SXAVENGER_ENGINE_NAMESPACE_END
