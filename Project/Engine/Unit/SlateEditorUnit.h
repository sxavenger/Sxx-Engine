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
#include <Runtime/Editor/Slate/SlateEditorPanel.h>
#include <Runtime/Editor/Slate/SlateEditorMenuBar.h>

//* lib
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Time/RunTimeTracker.h>
#include <Lib/Math/Vector2.h>

//* c++
#include <cstdint>
#include <cstddef>
#include <list>
#include <memory>
#include <concepts>
#include <string>
#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Slate editorの起動 / 更新 / 描画を行うUnit.
//! @note native windowはdockingの切り離し / 合体で増減するため, このUnitが所有して管理する.
//!       WindowUnitには依存しない. Framework::Context::GetUnit<T>()は未登録のUnitに対して
//!       nullptrを返すため, 他Unitに依存する設計にすると起動しない.
//! @note WindowUnitと同じconditionを登録するため, 両方をPushするとmessage pumpが二重に回る.
//!       どちらか一方のみ登録すること.
class SlateEditorUnit final
	: public Framework::IUnit {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// EditorWindow structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 1つのnative windowに対応する編集単位.
	//! @note ImGuiのcontextはnative windowごとに1つ必要なため, rendererもwindowごとに持つ.
	//!       hover / mouse captureもwindowごとに独立するため, applicationも1つずつ持つ.
	//! @note IDockingHostを実装し, tabの切り離し / tabを閉じる要求を受ける.
	//!       「どのwindowから引き出されたか」が要求と一緒に必要なため, hostはwindowごとに持つ.
	//! @note windows_(std::list)がunique_ptrで所有するためアドレスは安定する.
	//!       DockTabStackへ渡すhostと, 遅延要求が持つsourceのポインタはこの前提に依存する.
	struct EditorWindow final
		: public Editor::Slate::IDockingHost {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* docking host methods *//

		//! @brief このpanelを切り離してよいか.
		//! @note treeのpanelが1枚だけのsub windowは不可. 切り離すとwindowが空になって閉じられ,
		//!       閉じる途中に同じwindowへ戻す操作ができてtabごと消えるため.
		bool CanTearOff(const Editor::Slate::DockPanelPointer& panel) const override;

		//! @brief tabがtab barの外へ引き出された.
		//! @note DockTabStack::OnMouseMoveの中から呼ばれるため, 要求を積むだけに留める.
		//!       ここでwindowを生成したりtreeを差し替えると走査中のtreeを壊す.
		void RequestTearOff(Editor::Slate::DockPanelPointer panel, Vector2f screenPosition) override;

		//! @brief tabの × が押された.
		void RequestPanelClose(Editor::Slate::DockPanelPointer panel) override;

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* platform *//

		Application::Viewport viewport; //!< native windowとswap chain.

		bool isMain = false; //!< main windowかどうか. main windowを閉じるとapplicationが終了する.

		//* slate *//

		Editor::Slate::ImGuiRenderer renderer;    //!< このwindow専用のImGui context.
		Editor::Slate::Application   application; //!< layoutの駆動と入力routing.

		//!< 表示用のtree. TitleBarやMenuBarを含む.
		std::shared_ptr<Editor::Slate::Window> root = nullptr;

		//!< dockingのtree. SplitterとDockTabStackのみで構成される.
		//!< note: Docking::ApplyDockingHost / PruneEmptyNodes / FindDockTargetAt などの走査は
		//!<       DockTabStackとSplitterしか辿らない. rootにはchromeが挟まっており, rootを渡すと
		//!<       1段目で打ち切られてhostが配られない. そのため必ず分けて持つ.
		Editor::Slate::WidgetPointer dockRoot = nullptr;

		//* runtime parameter *//

		Vector2f mousePosition   = {}; //!< client座標系のマウス位置.
		bool     mouseButtons[3] = {}; //!< 前frameのマウスボタンの押下状態. (edge検出用)

		//* docking *//

		SlateEditorUnit* owner = nullptr; //!< 要求の実処理を行うUnit. (treeの差し替えはUnitの責務)

		//!< drag中に表示するdrop先. zone == Noneならマーカーのみを描く.
		Editor::Slate::DockTarget dockPreview = {};

	};

	using EditorWindowPointer = std::unique_ptr<EditorWindow>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// TearOffRequest structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief tabの切り離し要求. 入力処理の途中では処理できないため一度積む.
	struct TearOffRequest {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		EditorWindow* source = nullptr; //!< 引き出し元. 処理前に破棄され得るため生存を確認して使う.

		Editor::Slate::DockPanelPointer panel = nullptr; //!< 引き出すpanel.

		Vector2f screenPosition = {}; //!< 引き出した位置. (screen座標)

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// PanelCloseRequest structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief tabを閉じる要求.
	struct PanelCloseRequest {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		EditorWindow* source = nullptr; //!< 閉じるtabを持つwindow.

		Editor::Slate::DockPanelPointer panel = nullptr; //!< 閉じるpanel.

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// WindowDrag structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Unitが自前で行っているwindowの移動状態.
	//! @note OSのtitle barでの移動(HTCAPTION)はmodalなmove loopに入り, その間message pumpは
	//!       OS側で回るためUnitのUpdate / Renderが走らない. drop先のoverlayを出せず
	//!       ドッキングの判定もできないため, sub windowはUnitがSetWindowPosで動かす.
	struct WindowDrag {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		EditorWindow* window = nullptr; //!< 移動中のwindow. (nullptr = 移動していない)

		Vector2i grabOffset = {}; //!< 掴んだ点のwindow矩形左上からのoffset.

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* unit methods *//

	void Setup(Framework::Pipeline& pipeline) override;

	//* window option *//

	//! @brief editor windowを生成する.
	//! @retval 生成したwindow. 失敗した場合はnullptr.
	RefPtr<EditorWindow> CreateEditorWindow(const std::wstring& name, const Vector2ui& client, bool isMain = false);

	//! @brief editor windowの破棄を予約する.
	//! @note frameの途中でlistから消すとiteratorが壊れるため, EndFrameで回収する.
	void RequestCloseEditorWindow(RefPtr<EditorWindow> window);

	RefPtr<EditorWindow> GetMainEditorWindow() const;

	//* panel option *//

	//! @brief EditorPanelを継承したpanelをtabとして追加する.
	//! @note panelの寿命はUnitが持つ. tabを閉じてもインスタンスは破棄されないため,
	//!       同じpanelを後から開き直せる.
	//! @param window 追加先. nullptrならmain window.
	//! @retval 追加したpanel. 追加に失敗した場合はnullptr.
	template <class T, class... Args>
		requires std::derived_from<T, Editor::Slate::EditorPanel>
	std::shared_ptr<T> AddPanel(Args&&... args);

	//! @brief 生成済みのpanelをtabとして追加する.
	void AddPanel(const Editor::Slate::EditorPanelPointer& panel, RefPtr<EditorWindow> window = nullptr);

	//! @brief Unitが保持しているpanelの一覧.
	const std::vector<Editor::Slate::EditorPanelPointer>& GetPanels() const { return panels_; }

	//* menu bar option *//

	//! @brief EditorMenuBarを継承したmain menu barを設定する.
	//! @note main windowのchromeへ組み込まれる. 設定しなければmenu barは出ない.
	//! @retval 設定したmenu bar.
	template <class T, class... Args>
		requires std::derived_from<T, Editor::Slate::EditorMenuBar>
	std::shared_ptr<T> SetMenuBar(Args&&... args);

	//! @brief 生成済みのmenu barを設定する.
	void SetMenuBar(const Editor::Slate::EditorMenuBarPointer& menuBar);

	const Editor::Slate::EditorMenuBarPointer& GetMenuBar() const { return menuBar_; }

	const std::list<EditorWindowPointer>& GetEditorWindows() const { return windows_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* constant *//

	static constexpr size_t kMouseButtonCount = 3; //!< pollingするマウスボタンの数.

	static constexpr Vector2ui kDefaultClientSize = { 1280, 720 }; //!< main windowの既定サイズ.
	static constexpr Vector2ui kTearOffClientSize = { 640, 480 };  //!< 切り離したwindowの既定サイズ.

	static constexpr int32_t kTearOffCursorOffsetX = 80; //!< 掴んだままdragを続けられるようにする横offset.

	//!< マーカーを外していてもdropできるようにする縁の割合.
	//!< 領域の端からこの比率までが上下左右, それより内側は中央(tabとして追加).
	static constexpr float kDockEdgeRatio = 0.28f;

	//!< drag中のwindowの不透明度. 下のマーカーが見えるように薄くする.
	static constexpr uint8_t kWindowDragAlpha = 160;

	//* windows *//

	std::list<EditorWindowPointer> windows_;       //!< 所有しているeditor window.
	std::list<EditorWindow*>       closeRequests_; //!< 破棄が予約されたwindow.

	uint32_t windowSerial_ = 0; //!< window class名の衝突を避けるための連番.

	//* docking requests *//

	std::vector<TearOffRequest>     tearOffRequests_;
	std::vector<PanelCloseRequest>  panelCloseRequests_;

	WindowDrag windowDrag_ = {}; //!< 自前で移動中のwindow.

	bool isPreviousLeftButtonDown_ = false; //!< windowのdrag開始のedge検出用.

	//* panels *//

	//!< EditorPanelを継承したpanelの所有者. widgetはweak_ptrで参照するため,
	//!< ここが唯一の所有者になる. tabを閉じてもインスタンスは残る.
	std::vector<Editor::Slate::EditorPanelPointer> panels_;

	//!< main menu bar. 設定されていなければchromeへ積まない.
	Editor::Slate::EditorMenuBarPointer menuBar_ = nullptr;

	//* runtime parameter *//

	RunTimeTracker frameTracker_; //!< frame間の時間を計測するtracker.

	//!< 起動からの累計時間. Widget::PaintArguments::timeへ渡す. (deltaTimeではアニメーションが進まない)
	TimePointf<TimeUnit::Second> elapsedTime_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* unit helper methods *//

	void InitEditor();

	void UpdateEditor();

	void RenderEditor();

	void PresentEditor();

	void TermEditor();

	//! @brief loopを抜ける条件. message pumpもここで回す.
	bool IsRequestExit();

	//* frame helper methods *//

	void RenderWindow(EditorWindow& window, TimePointf<TimeUnit::Second> deltaTime);

	//! @brief drag中のdrop先マーカーとプレビューをForegroundへ描く.
	void RenderDockOverlay(EditorWindow& window);

	//! @brief マウスをpollingし, ImGuiとSlateへイベントを配送する.
	void UpdateInput(EditorWindow& window);

	//* chrome helper methods *//

	//! @brief OSのtitle barを消し, 自前のTitleBarで移動 / リサイズできるように配線する.
	void SetupWindowChrome(EditorWindow& window);

	//! @brief borderless windowのヒットテスト. (Platform::Windowから呼ばれる)
	Platform::Window::NonClientArea HitTestWindowChrome(const EditorWindow& window, const Vector2i& client) const;

	//! @brief borderless windowのカーソル形状. (Platform::Windowから呼ばれる)
	Platform::Window::CursorShape QueryWindowCursor(const EditorWindow& window) const;

	//! @brief TitleBarのボタンを配線したwidgetを返す.
	Editor::Slate::WidgetPointer CreateTitleBar(EditorWindow& window, bool isButtonsOnly);

	//! @brief dockRootをchromeで包み直してrootへ設定する.
	//! @note dockRootを差し替えたら必ず呼ぶ.
	void RebuildWindowChrome(EditorWindow& window);

	//! @brief widget treeを構築する.
	void BuildLayout(EditorWindow& window);

	//* docking helper methods *//

	void EnqueueTearOff(EditorWindow* source, Editor::Slate::DockPanelPointer panel, Vector2f clientPosition);

	void EnqueuePanelClose(EditorWindow* source, Editor::Slate::DockPanelPointer panel);

	void ProcessPendingRequests();

	void ProcessTearOffRequest(const TearOffRequest& request);

	void ProcessPanelCloseRequest(const PanelCloseRequest& request);

	//! @brief 破棄されるwindowを指す要求とdrag状態を捨てる.
	void DiscardRequests(EditorWindow* window);

	bool IsAliveEditorWindow(const EditorWindow* window) const;

	//! @brief treeの全DockTabStackへhostを配り直す.
	void ApplyDockingHostToWindow(EditorWindow& window);

	void AddPanelToEditorWindow(EditorWindow& window, const Editor::Slate::DockPanelPointer& panel);

	//! @brief panelを全て失ったsub windowを閉じる. (mainは残す)
	void CloseIfEmptyEditorWindow(EditorWindow& window);

	//! @brief 破棄が予約された / OSに閉じられたwindowを回収する.
	void CollectClosedWindows();

	//* window drag helper methods *//

	void UpdateWindowDrag();

	bool TryBeginWindowDrag(const Vector2i& cursor);

	void BeginWindowDrag(EditorWindow& window, const Vector2i& cursor);

	void EndWindowDrag(const Vector2i& cursor);

	void UpdateDockPreview(EditorWindow* dragged, const Vector2i& cursor);

	void ClearDockPreviews();

	void MoveEditorWindow(EditorWindow& window, const Vector2i& position);

	EditorWindow* FindEditorWindowUnderCursor(const EditorWindow* dragged, const Vector2i& cursor);

	//! @brief drop先のゾーンを決める.
	//! @note マーカーの上に居ればそれを使い, 外していても領域内の位置から推定する.
	//!       マーカーだけを条件にすると狙いが厳しすぎて実用にならない.
	static Editor::Slate::DockZone ResolveDockZone(const Editor::Slate::DockTarget& target, const Vector2f& client);

	//! @brief drag中のwindowを半透明にする. (下に隠れたマーカーを見えるようにする)
	static void SetWindowDragOpacity(EditorWindow& window, bool isDragging);

	//* input helper methods *//

	//! @brief 仮想keyが押下されているかを返す.
	//! @note Editor専用のpolling. Platform::Inputは使わない.
	//!       (DirectInput経由ではGetDeviceStateの失敗を拾えずclickが死ぬ事例があったため)
	static bool IsVirtualKeyDown(int32_t key);

	//! @brief dockRootが実際に置かれている矩形を返す.
	//! @note chromeの分だけclientより小さい. ここがずれるとマーカーの描画位置と当たり判定がずれる.
	//! @note menu barの有無を見るためstaticにできない.
	Editor::Slate::Geometry GetDockArea(const EditorWindow& window) const;

	//! @brief main windowのchromeがclientの上端から占める高さ.
	float GetChromeTopHeight(const EditorWindow& window) const;

	static bool CanDockInto(const EditorWindow* dragged, const EditorWindow* target);

	static bool ContainsScreenPoint(const EditorWindow& window, const Vector2i& cursor);

	//* input helper methods *//

};

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class... Args>
	requires std::derived_from<T, Editor::Slate::EditorPanel>
std::shared_ptr<T> SlateEditorUnit::AddPanel(Args&&... args) {

	std::shared_ptr<T> panel = std::make_shared<T>(std::forward<Args>(args)...);

	AddPanel(panel, nullptr);

	return panel;
}

template <class T, class... Args>
	requires std::derived_from<T, Editor::Slate::EditorMenuBar>
std::shared_ptr<T> SlateEditorUnit::SetMenuBar(Args&&... args) {

	std::shared_ptr<T> menuBar = std::make_shared<T>(std::forward<Args>(args)...);

	SetMenuBar(Editor::Slate::EditorMenuBarPointer(menuBar));

	return menuBar;
}

SXAVENGER_ENGINE_NAMESPACE_END
