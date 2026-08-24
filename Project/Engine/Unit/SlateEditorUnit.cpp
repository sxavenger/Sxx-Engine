#include "SlateEditorUnit.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine [graphics]
#include <Runtime/Graphics/Core.h>

//* engine [editor]
#include <Runtime/Editor/Slate/SlateIcon.h>
#include <Runtime/Editor/Slate/SlateTypes.h>
#include <Runtime/Editor/Slate/Style/SlateStyle.h>
#include <Runtime/Editor/Slate/Style/SlateStyleEditor.h>
#include <Runtime/Editor/Slate/Widgets/SlateDecl.h>
#include <Runtime/Editor/Slate/Widgets/SlateBoxPanel.h>
#include <Runtime/Editor/Slate/Widgets/SlateOverlay.h>
#include <Runtime/Editor/Slate/Widgets/SlateTitleBar.h>
#include <Runtime/Editor/Slate/Widgets/SlateImGuiWidget.h>
#include <Runtime/Editor/Slate/Widgets/SlateImGuiMenuBar.h>
#include <Runtime/Editor/Slate/Docking/SlateDockPanel.h>
#include <Runtime/Editor/Slate/Docking/SlateDockTabStack.h>
#include <Runtime/Editor/Slate/Docking/SlateSplitter.h>
#include <Runtime/Editor/Slate/SlateEditorPanel.h>
#include <Runtime/Editor/Slate/SlateEditorMenuBar.h>
#include <Runtime/Editor/EditorTestPanel.h>
#include <Runtime/Editor/EditorTestMainMenu.h>

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/String/UnicodeConverter.h>
#include <Lib/Time/TimePoint.h>

//* imgui
#include <imgui.h>

//* windows
#include <windows.h>

//* c++
#include <algorithm>
#include <format>
#include <string>

//-----------------------------------------------------------------------------------------
// constant
//-----------------------------------------------------------------------------------------

namespace {

	//!< back bufferのformat.
	//!< note: SwapChainがRTVを ConvertToSRGBFormat() で作るため, ImGuiRendererのPSOも同じ変換を通している.
	//!< TODO: Configurationから取得できるようにする.
	constexpr DXGI_FORMAT kBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

}

//-----------------------------------------------------------------------------------------
// helper
//-----------------------------------------------------------------------------------------

namespace {

	//!< cursorのscreen座標を取得する.
	Vector2i GetCursorPosition() {

		POINT point = {};

		if (::GetCursorPos(&point) == FALSE) {
			return {};
		}

		return { static_cast<int32_t>(point.x), static_cast<int32_t>(point.y) };
	}

	//!< EditorPanelを継承したpanelの実例.
	//!< OnDraw()の中はImGuiの呼び出しを並べるだけでよい. Begin / Endは基底が面倒を見る.
	class StyleEditorPanel final
		: public Editor::Slate::EditorPanel {
	public:

		StyleEditorPanel() {
			SetTitle(std::format("{} Style", Editor::Slate::Icon::Settings));
		}

		void OnDraw() override {
			Editor::Slate::StyleEditor::Draw();
		}

	};

	//!< screen座標をclient座標へ変換する.
	//!< @retval false client領域の外.
	bool ToClientPosition(HWND hwnd, const Vector2i& screen, Vector2f& out) {

		POINT point = { static_cast<LONG>(screen.x), static_cast<LONG>(screen.y) };

		if (hwnd == nullptr || ::ScreenToClient(hwnd, &point) == FALSE) {
			return false;
		}

		RECT client = {};

		if (::GetClientRect(hwnd, &client) == FALSE) {
			return false;
		}

		if (point.x < client.left || point.x >= client.right || point.y < client.top || point.y >= client.bottom) {
			return false;
		}

		out = { static_cast<float>(point.x), static_cast<float>(point.y) };
		return true;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// [SlateEditorUnit] EditorWindow structure methods
////////////////////////////////////////////////////////////////////////////////////////////

bool SlateEditorUnit::EditorWindow::CanTearOff(const Editor::Slate::DockPanelPointer& panel) const {

	if (panel == nullptr || dockRoot == nullptr) {
		return false;
	}

	//!< main windowは空になっても閉じないため, 常に引き出してよい.
	if (isMain) {
		return true;
	}

	//!< sub windowの最後の1枚は引き出させない. 引き出すとwindowが空になって閉じられ,
	//!< 閉じる途中に同じwindowへ戻す操作ができてtabごと消えるため. (windowごと動かせばよい)
	return Editor::Slate::CollectAllPanels(dockRoot).size() > 1;
}

void SlateEditorUnit::EditorWindow::RequestTearOff(Editor::Slate::DockPanelPointer panel, Vector2f screenPosition) {

	if (owner == nullptr) {
		return;
	}

	//!< 要求を積むだけに留める. この関数はDockTabStack::OnMouseMoveの中から呼ばれるため,
	//!< ここでwindowを生成したりtreeを差し替えると走査中のtreeを壊す.
	owner->EnqueueTearOff(this, std::move(panel), screenPosition);
}

void SlateEditorUnit::EditorWindow::RequestPanelClose(Editor::Slate::DockPanelPointer panel) {

	if (owner == nullptr) {
		return;
	}

	//!< RequestTearOffと同じ理由で遅延させる. (OnMouseButtonDownの中から呼ばれる)
	owner->EnqueuePanelClose(this, std::move(panel));
}

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SlateEditorUnit::Setup(Framework::Pipeline& pipeline) {

	//!< window生成はGraphics::Coreの初期化後でなければならないため, Priority::Lowestで後に走らせる.
	pipeline.SetProcess(Framework::Phase::Initialize, Framework::Priority::Lowest, [this]() { InitEditor(); });

	//!< 遅延要求の処理, swap chainのresize追従, 入力のpolling, windowのdrag.
	pipeline.SetProcess(Framework::Phase::Update, Framework::Priority::Normal, [this]() { UpdateEditor(); });

	//!< 全windowのlayoutと描画.
	pipeline.SetProcess(Framework::Phase::Render, Framework::Priority::Lowest, [this]() { RenderEditor(); });

	//!< 全windowのPresentと, 閉じられたwindowの回収.
	pipeline.SetProcess(Framework::Phase::EndFrame, Framework::Priority::Normal, [this]() { PresentEditor(); });

	//!< Phase::Terminateはpriorityの逆順で実行されるため, Priority::LowestがTerminateの最初に走る.
	pipeline.SetProcess(Framework::Phase::Terminate, Framework::Priority::Lowest, [this]() { TermEditor(); });

	//!< loopを抜ける条件. message pumpもここで回す.
	//!< note: WindowUnitも同じconditionを登録するため, 併用するとmessage pumpが二重に回る.
	pipeline.SetCondition([this]() { return IsRequestExit(); });
}

void SlateEditorUnit::InitEditor() {

	//!< main windowを生成する. 以降のsub windowは切り離しから生成される.
	RefPtr<EditorWindow> window = CreateEditorWindow(L"[Sxavenger Engine] Slate Editor", kDefaultClientSize, true);

	if (window == nullptr) {
		STREAM_ASSERT(false, "SlateEditorUnit | failed to create main editor window.");
		return;
	}

	//!< 既定のmain menu bar. 差し替えるなら SetMenuBar<T>() を呼ぶ.
	SetMenuBar<Editor::EditorTestMainMenu>();

	BuildLayout(*window);

	frameTracker_.Start(); //!< frame間の時間の計測を開始する.

	STREAM_LOG_INFO("SlateEditorUnit | slate editor initialized.");
}

void SlateEditorUnit::UpdateEditor() {

	//!< 入力処理中に積まれた要求(切り離し / tabを閉じる)をここでまとめて処理する.
	//!< 入力の配送より前に行う理由は, 差し替えが終わったtreeへ入力を流すため, および同じframeで
	//!< 生成したwindowにもresizeの追従と入力の配送を行き渡らせるため.
	//!< note: windows_はstd::listなので, 処理中にwindowが追加されても下のloopは壊れない.
	ProcessPendingRequests();

	for (const EditorWindowPointer& window : windows_) {

		if (!window->viewport.GetWindow().IsOpen()) {
			continue; //!< OSに閉じられたwindowは触らない. PresentEditorで回収する.
		}

		//!< SwapChain::Resizeはback bufferを解放するがGPUの完了を待たない.
		//!< 待たずに解放すると EXECUTION ERROR #921 になるため, resizeが起きるframeだけ先に待つ.
		//!< note: SubmitDirectQueue -> ExecuteAll -> Reset -> WaitGpu でCPU側が待機する.
		if (window->viewport.GetWindow().GetEvent() == Platform::Window::Event::Resize) {
			Graphics::Core::SubmitDirectQueueWait();
		}

		window->viewport.Update(); //!< resizeが発生していればswap chainを追従させる.

		//!< eventは消費されるまで保持されるため, 処理したらここで消す.
		//!< 消さないとResizeが残り続け, 毎frame swap chainを再生成してしまう.
		window->viewport.GetWindow().ConsumeEvent();

		if (window->root != nullptr) {
			window->root->SetClientSize(static_cast<Vector2f>(window->viewport.GetClient()));
		}

		UpdateInput(*window);
	}

	//!< main menu barの更新.
	if (menuBar_ != nullptr) {
		menuBar_->OnUpdate();
	}

	//!< EditorPanelの更新. 描画に依存しない処理をここで回す.
	//!< note: tabが裏に隠れていても呼ぶ. 描画は OnDraw() 側でImGuiのframe内で行われる.
	for (const Editor::Slate::EditorPanelPointer& panel : panels_) {
		if (panel != nullptr) {
			panel->OnUpdate();
		}
	}

	//!< windowのdrag移動とドッキングの判定.
	//!< 入力の配送より後に行う理由:
	//!<   1. dropはwidget treeを差し替えるため, widgetの走査の外でなければならない.
	//!<   2. 切り離しで生成したwindowをこのframeのうちにcursorへ追従させ, 1frameの飛びを避ける.
	UpdateWindowDrag();
}

void SlateEditorUnit::RenderEditor() {

	if (windows_.empty()) {
		return;
	}

	//!< frame間の時間を求める. RunTimeTrackerはmicrosecondで計測するためsecondへ変換する.
	frameTracker_.Stop();
	const TimePointd<TimeUnit::Microsecond> measured = frameTracker_.GetMeasuredTime();
	frameTracker_.Start();

	const TimePointf<TimeUnit::Second> deltaTime = measured;
	//!< note: 0を渡してもImGuiRenderer::BeginFrame側でクランプされるため, ここでのクランプは行わない.

	//!< Widget::PaintArguments::timeはアニメーションの位相に使うため, deltaTimeではなく累計時間を渡す.
	elapsedTime_ += deltaTime;

	for (const EditorWindowPointer& window : windows_) {

		if (!window->viewport.GetWindow().IsOpen() || window->root == nullptr) {
			continue;
		}

		RenderWindow(*window, deltaTime);
	}

	//!< note: queueへの投入はここでは行わない.
	//!<       EngineUnitが Phase::EndFrame / Priority::Highest で SubmitDirectQueue() を呼ぶ.
	//!<       ここでも投入すると二重になり, 空のcommand listを実行して余計にGPUを待つ.
	//!<       順序は Render(記録) -> EndFrame/Highest(投入) -> EndFrame/Normal(Present) で正しい.
	//!< note: EngineUnitが使う SubmitDirectQueue() は ExecuteAll() なので毎frame GPUの完了を待つ.
	//!<       CPUとGPUを重ねたい場合はEngineUnit側を ExecuteAdvance() にし,
	//!<       Packages/config/Graphics.toml の allocatorCount を kFrameCount 以上にする.
}

void SlateEditorUnit::RenderWindow(EditorWindow& window, TimePointf<TimeUnit::Second> deltaTime) {

	Graphics::GraphicsCommandContext& context = Graphics::Core::GetCommandContextDirect();
	Graphics::SwapChain::Buffer&      buffer  = window.viewport.GetCurrentBackBuffer();

	//!< back bufferをrender targetとして設定する.
	//!< note: clearもsRGB RTVへの書き込みなので, linearへ戻してから渡す. (二重ガンマを避ける)
	buffer.TransitionRenderTarget(context);
	buffer.ClearRenderTarget(
		context, Editor::Slate::ConvertToLinearColor(Editor::Slate::Style::GetStyle().colors.background)
	);
	buffer.OMSetRenderTarget(context);

	//!< Slateの描画. ImGuiのcontextはwindowごとに独立しているため, frameの開始 / 終了もwindowごとに行う.
	window.renderer.BeginFrame(window.root->GetClientSize(), deltaTime);
	window.application.Tick(*window.root, &window.renderer, elapsedTime_);
	RenderDockOverlay(window);
	window.renderer.EndFrame(context);

	buffer.TransitionPresent(context);
}

void SlateEditorUnit::RenderDockOverlay(EditorWindow& window) {

	if (windowDrag_.window == nullptr || windowDrag_.window == &window) {
		return; //!< drag中でない, もしくは自分自身が動いている場合はマーカーを出さない.
	}

	if (!CanDockInto(windowDrag_.window, &window)) {
		return;
	}

	const Editor::Slate::Geometry area = GetDockArea(window);

	if (area.localSize.x <= 0.0f || area.localSize.y <= 0.0f) {
		return;
	}

	//!< ドッキングのオーバーレイは最前面へ描く.
	window.renderer.SetDrawTarget(Editor::Slate::ImGuiRenderer::DrawTarget::Foreground);

	const Editor::Slate::Style& style = Editor::Slate::Style::GetStyle();

	const Editor::Slate::Geometry target = window.dockPreview.IsValid() ? window.dockPreview.area : area;

	//!< 1) 対象のtab stack全体をうっすら示す. どのpanelへ入るのかが分かるようにする.
	//!<    マーカーの上に居ないときも出すことで, drop先が決まっていないことも伝わる.
	window.renderer.DrawRect(target, style.colors.activeSoft);
	window.renderer.DrawBorder(target, style.colors.active, style.metrics.borderThin);

	//!< 2) drop結果の領域.
	if (window.dockPreview.zone != Editor::Slate::DockZone::None) {
		const Editor::Slate::Geometry preview
			= Editor::Slate::DockZonePreviewGeometry(target, window.dockPreview.zone);

		window.renderer.DrawRect(preview, style.colors.activeSoft);
		window.renderer.DrawBorder(preview, style.colors.active, style.metrics.borderThick);

		//!< 中央(tabとして追加)のときはtab barを強調する.
		//!< 領域全体が同じ塗りになるため, 上下左右との区別が付かないのを避ける.
		if (window.dockPreview.zone == Editor::Slate::DockZone::Center) {

			const Editor::Slate::Geometry tabBar = {
				target.absolutePosition,
				{ target.localSize.x, Editor::Slate::DockTabStack::TabBarHeight() },
				target.scale
			};

			window.renderer.DrawRect(tabBar, style.colors.active);
		}
	}

	//!< 3) 5つのゾーンマーカー. 描画と当たり判定で同じ BuildDockZoneMarkers を使う.
	Editor::Slate::DockZoneMarker markers[Editor::Slate::kDockZoneMarkerCount] = {};
	Editor::Slate::BuildDockZoneMarkers(target, markers);

	for (const Editor::Slate::DockZoneMarker& marker : markers) {

		const bool isHovered = (marker.zone == window.dockPreview.zone);

		window.renderer.DrawRoundedRect(
			marker.box, isHovered ? style.colors.active : style.colors.panel, style.metrics.tabRounding
		);
		window.renderer.DrawBorder(
			marker.box, isHovered ? style.colors.text : style.colors.border, style.metrics.borderThin
		);

		//!< マーカーの中に結合結果を示す図を描く. これが無いとどのゾーンか判別できない.
		//!< 中央 = tabとして追加(枠のみ) / 上下左右 = 分割される側を塗る.
		//!< note: 移植元は colors.white を使っていたが engine には無いため colors.text で代用する.
		const Color4f ink = isHovered ? style.colors.text : style.colors.textDim;

		const float padding = marker.box.localSize.x * 0.22f;

		const Editor::Slate::Geometry inner = {
			{ marker.box.absolutePosition.x + padding, marker.box.absolutePosition.y + padding },
			{ marker.box.localSize.x - padding * 2.0f, marker.box.localSize.y - padding * 2.0f },
			target.scale
		};

		window.renderer.DrawBorder(inner, ink, style.metrics.borderThin);

		if (marker.zone != Editor::Slate::DockZone::Center) {
			window.renderer.DrawRect(Editor::Slate::DockZonePreviewGeometry(inner, marker.zone), ink);
		}
	}

	window.renderer.SetDrawTarget(Editor::Slate::ImGuiRenderer::DrawTarget::Background);
}

void SlateEditorUnit::PresentEditor() {

	for (const EditorWindowPointer& window : windows_) {

		if (!window->viewport.GetWindow().IsOpen()) {
			continue;
		}

		window->viewport.Present(false);
	}

	CollectClosedWindows(); //!< Presentの後に回収する. (描画中のwindowを破棄しないため)
}

void SlateEditorUnit::TermEditor() {

	closeRequests_.clear();
	tearOffRequests_.clear();
	panelCloseRequests_.clear();
	windowDrag_ = {};

	//!< 全windowのGPU resourceを解放する前にGPUの完了を待つ. (理由はCollectClosedWindowsと同じ)
	if (!windows_.empty()) {
		Graphics::Core::SubmitDirectQueueWait();
	}

	for (const EditorWindowPointer& window : windows_) {
		window->dockRoot = nullptr;
		window->root.reset();                 //!< widget treeを先に破棄する.
		window->renderer.Shutdown();          //!< ImGuiのcontextはwidgetの後に破棄する.
		window->viewport.GetWindow().Reset(); //!< DestroyWindowとUnregisterClassを行う.
	}

	windows_.clear(); //!< ここでViewport(swap chain)が破棄される.
}

bool SlateEditorUnit::IsRequestExit() {

	//!< message pumpは全windowで共有されるため, 1frameに1度だけ回す.
	if (!Platform::Window::ProcessMessage()) {
		return true; //!< WM_QUITを受け取った. (main windowが破棄された)
	}

	//!< 全てのeditor windowが失われた場合も終了する.
	return windows_.empty();
}

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class window methods
////////////////////////////////////////////////////////////////////////////////////////////

RefPtr<SlateEditorUnit::EditorWindow> SlateEditorUnit::CreateEditorWindow(const std::wstring& name, const Vector2ui& client, bool isMain) {

	if (isMain && GetMainEditorWindow() != nullptr) {
		STREAM_LOG_WARNING("SlateEditorUnit | main editor window already exists.");
		return nullptr;
	}

	EditorWindowPointer window = std::make_unique<EditorWindow>();
	window->isMain = isMain;
	window->owner  = this; //!< 切り離し / 閉じる要求の実処理はUnitが行う.

	//!< Platform::Windowはwindow名からwindow classを作り, RegisterClassの失敗はexceptionになる.
	//!< 同じtitleのpanelを二度切り離すと名前が衝突するため, class名の元に一意な連番を付ける.
	//!< 表示上のtitleは生成後にSetWindowTextWで戻す.
	const std::wstring identifier = name + L"##" + std::to_wstring(windowSerial_++);

	//!< main windowの破棄はWM_QUITを送るが, sub windowは送らない.
	//!< OSのtitle barは出さず, Slate::TitleBarで描く.
	window->viewport.Create(
		identifier, client,
		isMain ? Platform::Window::Category::Main : Platform::Window::Category::Sub,
		kBackBufferFormat,
		Platform::Window::Style::Borderless
	);

	if (const HWND hwnd = window->viewport.GetWindow().GetHwnd(); hwnd != nullptr) {
		::SetWindowTextW(hwnd, name.c_str());
	}

	window->renderer.Init(); //!< ImGuiのcontextはwindowごとに1つ生成する.

	window->root = std::make_shared<Editor::Slate::Window>();
	window->root->SetClientSize(static_cast<Vector2f>(window->viewport.GetClient()));

	SetupWindowChrome(*window);

	windows_.push_back(std::move(window));

	STREAM_LOG_INFO(L"SlateEditorUnit | create editor window. name: {}", name);

	return windows_.back().get();
}

void SlateEditorUnit::RequestCloseEditorWindow(RefPtr<EditorWindow> window) {

	if (window == nullptr) {
		return;
	}

	//!< frameの途中でlistから消すとiteratorが壊れるため, 予約だけ行う.
	closeRequests_.push_back(window.Get());
}

RefPtr<SlateEditorUnit::EditorWindow> SlateEditorUnit::GetMainEditorWindow() const {

	for (const EditorWindowPointer& window : windows_) {
		if (window->isMain) {
			return window.get();
		}
	}

	return nullptr;
}

void SlateEditorUnit::CollectClosedWindows() {

	//!< OSに閉じられたsub windowも回収対象にする.
	for (const EditorWindowPointer& window : windows_) {

		if (window->isMain) {
			continue; //!< main windowはWM_QUITでloopが終了するため, ここでは回収しない.
		}

		if (!window->viewport.GetWindow().IsOpen()) {
			closeRequests_.push_back(window.get());
		}
	}

	if (closeRequests_.empty()) {
		return;
	}

	//!< swap chainのback bufferとImGuiのGPU resourceを解放する前にGPUの完了を待つ.
	//!< 直前のPresentがback bufferを参照したままなので, 待たずに解放すると
	//!< EXECUTION ERROR #921 (OBJECT_DELETED_WHILE_STILL_IN_USE) になる.
	Graphics::Core::SubmitDirectQueueWait();

	for (EditorWindow* request : closeRequests_) {

		const auto it = std::find_if(windows_.begin(), windows_.end(), [request](const EditorWindowPointer& window) {
			return window.get() == request;
		});

		if (it == windows_.end()) {
			continue; //!< すでに回収済み. (二重に予約された場合)
		}

		//!< このwindowを指す遅延要求とdrag状態を捨てる. 残すとerase後にdangling pointerを触る.
		DiscardRequests(request);

		(*it)->dockRoot = nullptr;
		(*it)->root.reset();
		(*it)->renderer.Shutdown();

		//!< Viewport::CloseはCloseWindow(= 最小化)を呼ぶだけでwindowを破棄しない.
		//!< 破棄しないとGWLP_USERDATAが解放済みのWindow*を指したままmessageを受け, use after freeになる.
		//!< Window::ResetはDestroyWindowとUnregisterClassを行うため, こちらを使う.
		(*it)->viewport.GetWindow().Reset();

		windows_.erase(it);
	}

	closeRequests_.clear();
}

bool SlateEditorUnit::IsAliveEditorWindow(const EditorWindow* window) const {

	if (window == nullptr) {
		return false;
	}

	return std::any_of(windows_.begin(), windows_.end(), [window](const EditorWindowPointer& entry) {
		return entry.get() == window;
	});
}

void SlateEditorUnit::DiscardRequests(EditorWindow* window) {

	if (window == nullptr) {
		return;
	}

	std::erase_if(tearOffRequests_, [window](const TearOffRequest& request) {
		return request.source == window;
	});

	std::erase_if(panelCloseRequests_, [window](const PanelCloseRequest& request) {
		return request.source == window;
	});

	if (windowDrag_.window == window) {
		windowDrag_ = {};
		ClearDockPreviews();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class chrome methods
////////////////////////////////////////////////////////////////////////////////////////////

void SlateEditorUnit::SetupWindowChrome(EditorWindow& window) {

	Platform::Window& native = window.viewport.GetWindow();

	EditorWindow* pointer = &window; //!< windows_(std::list)が所有するためアドレスは安定する.

	native.SetNonClientHitTest([this, pointer](const Vector2i& client) {
		return HitTestWindowChrome(*pointer, client);
	});

	native.SetCursorQuery([this, pointer](const Vector2i& /*client*/) {
		return QueryWindowCursor(*pointer);
	});
}

Platform::Window::NonClientArea SlateEditorUnit::HitTestWindowChrome(const EditorWindow& window, const Vector2i& client) const {

	using NonClientArea = Platform::Window::NonClientArea;

	//!< sub windowはtab行もClientのままにする.
	//!< HTCAPTIONを返すとOSのmodalなmove loopへ入り, その間Update / Renderが走らない.
	//!< drop先のoverlayを描けずドッキングの判定もできないため, sub windowの移動はUnitが
	//!< SetWindowPosで自前に行う. (UpdateWindowDrag)
	if (!window.isMain || window.root == nullptr) {
		return NonClientArea::Client;
	}

	const Vector2f position = { static_cast<float>(client.x), static_cast<float>(client.y) };

	//!< title barの行より下はwidgetが入力を受ける.
	if (position.y >= Editor::Slate::TitleBar::Height()) {
		return NonClientArea::Client;
	}

	//!< 最小化 / 最大化 / 閉じるボタンの上はwidgetへ渡す. Captionにするとclickが押下判定にならない.
	const float width = window.root->GetClientSize().x;

	if (Editor::Slate::TitleBar::HitTestButton(position, width) != Editor::Slate::TitleBar::Button::None) {
		return NonClientArea::Client;
	}

	//!< main windowは他windowへドッキングしないため, 移動はOSに任せてよい.
	return NonClientArea::Caption;
}

Platform::Window::CursorShape SlateEditorUnit::QueryWindowCursor(const EditorWindow& window) const {

	using CursorShape = Platform::Window::CursorShape;

	//!< Slateが決めたカーソル形状をPlatformの種別へ変換する. (Platform層はSlateに依存できない)
	switch (window.application.GetCurrentCursor()) {
		case Editor::Slate::Cursor::ResizeLeftRight: return CursorShape::SizeWE;
		case Editor::Slate::Cursor::ResizeUpDown:    return CursorShape::SizeNS;
		case Editor::Slate::Cursor::Hand:            return CursorShape::Hand;
		case Editor::Slate::Cursor::TextEdit:        return CursorShape::IBeam;
		default:                                     return CursorShape::Arrow;
	}
}

Editor::Slate::WidgetPointer SlateEditorUnit::CreateTitleBar(EditorWindow& window, bool isButtonsOnly) {

	const HWND hwnd = window.viewport.GetWindow().GetHwnd();

	Editor::Slate::Decl<Editor::Slate::TitleBar> titleBar;
	titleBar->SetButtonsOnly(isButtonsOnly);

	if (!isButtonsOnly) {
		titleBar->SetTitle("Sxavenger Engine");
	}

	titleBar->SetOnMinimize([hwnd]() { ::ShowWindow(hwnd, SW_MINIMIZE); });

	titleBar->SetOnMaximize([hwnd]() {
		//!< 現在の状態を見てトグルする.
		::ShowWindow(hwnd, (::IsZoomed(hwnd) != FALSE) ? SW_RESTORE : SW_MAXIMIZE);
	});

	//!< main windowはWM_DESTROYでWM_QUITが送られるためloopが終了する.
	//!< sub windowはRequestCloseEditorWindowで回収する.
	if (window.isMain) {
		titleBar->SetOnClose([hwnd]() { ::PostMessageW(hwnd, WM_CLOSE, 0, 0); });

	} else {
		EditorWindow* pointer = &window;
		titleBar->SetOnClose([this, pointer]() { RequestCloseEditorWindow(pointer); });
	}

	return titleBar.pointer;
}

void SlateEditorUnit::RebuildWindowChrome(EditorWindow& window) {

	if (window.root == nullptr) {
		return;
	}

	//!< dockRootが空(全panelを失った)ならchromeだけを残す. main windowは土台として残るため必要.
	const Editor::Slate::WidgetPointer dock = window.dockRoot;

	if (window.isMain) {
		//!< main windowはTitleBarとMenuBarを縦に積む.
		//!< note: ImGuiMenuBarは自前でImGui::Beginするため, ImGuiWidgetのBeginRegionの内側に
		//!<       置いてはいけない. chromeとして外側に置く.
		Editor::Slate::Decl<Editor::Slate::VerticalBox> box;
		box->AddSlot().AutoSize().Content(CreateTitleBar(window, false));

		//!< menu barは設定されているときだけ積む.
		if (menuBar_ != nullptr) {
			box->AddSlot().AutoSize().Content(menuBar_->GetWidget());
		}

		if (dock != nullptr) {
			box->AddSlot().Fill(1.0f).Content(dock);
		}

		window.root->SetPointer(box.pointer);
		return;
	}

	//!< sub windowはTitleを出さない. tab barの右上にボタンだけを重ね, その分だけtabの領域を空ける.
	//!< note: 余白は「右上のtab stack」にだけ入れる. 分割していると全stackに入ってしまうため,
	//!<       一度resetしてから右上のstackへ再設定する.
	if (dock != nullptr) {
		Editor::Slate::ResetTabBarInsets(dock);

		if (const Editor::Slate::DockTabStackPointer stack = Editor::Slate::FindTopRightTabStack(dock)) {
			stack->SetTabBarRightInset(Editor::Slate::TitleBar::ButtonsWidth());
		}
	}

	Editor::Slate::Decl<Editor::Slate::Overlay> overlay;

	if (dock != nullptr) {
		overlay->AddSlot().Content(dock);
	}

	overlay->AddSlot()
		.HAlign(Editor::Slate::HAlign::Right)
		.VAlign(Editor::Slate::VAlign::Top)
		.Content(CreateTitleBar(window, true)); //!< ボタンのみ.

	window.root->SetPointer(overlay.pointer);
}

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class layout methods
////////////////////////////////////////////////////////////////////////////////////////////

void SlateEditorUnit::BuildLayout(EditorWindow& window) {

	if (window.root == nullptr) {
		return;
	}

	if (!window.isMain) {
		//!< sub windowは切り離されたpanelの受け皿として空のtab stackを置く.
		Editor::Slate::Decl<Editor::Slate::DockTabStack> stack;

		window.dockRoot = stack.pointer;
		RebuildWindowChrome(window);
		ApplyDockingHostToWindow(window);
		return;
	}

	//!< main windowのlayout. splitterで左右に分割し, それぞれにtab stackを置く.
	//!< note: アイコンは専用APIではなくtitleの文字列に埋め込む. 位置も個数も自由に決められる.

	Editor::Slate::Decl<Editor::Slate::DockTabStack> stack;
	{
		//!< EditorPanelを継承したpanelの例. titleとtab色はpanel側のconstructorで決まる.
		//!< note: layoutを組んでいる途中でdockRootがまだ無いためAddPanel()は使えない.
		//!<       ここではpanelを直接作ってtab stackへ入れ, 所有だけpanels_へ預ける.
		const Editor::Slate::EditorPanelPointer test = std::make_shared<Editor::EditorTestPanel>();
		panels_.push_back(test);
		stack->AddPanel(test->GetDockPanel());
	}

	{
		//!< Styleの調整タブ. EditorPanelを継承して作る例になっている.
		//!< note: layoutを組んでいる途中なのでdockRootがまだ無く, AddPanel()は使えない.
		//!<       ここではpanelを直接作ってtab stackへ入れ, 所有だけpanels_へ預ける.
		const Editor::Slate::EditorPanelPointer style = std::make_shared<StyleEditorPanel>();
		panels_.push_back(style);
		stack->AddPanel(style->GetDockPanel());
	}

	window.dockRoot = stack.pointer;
	RebuildWindowChrome(window); //!< chromeで包んでrootへ設定する.

	ApplyDockingHostToWindow(window); //!< 生成した全tab stackへ切り離し要求の通知先を配る.
}

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class panel methods
////////////////////////////////////////////////////////////////////////////////////////////

void SlateEditorUnit::SetMenuBar(const Editor::Slate::EditorMenuBarPointer& menuBar) {

	menuBar_ = menuBar;

	//!< 既に生成済みのmain windowがあればchromeを巻き直して反映する.
	//!< note: RefPtrをconstにすると operator*() const が const T& を返すため,
	//!<       非constの参照を取る関数へ渡せない. ここはconstを付けないこと.
	if (RefPtr<EditorWindow> window = GetMainEditorWindow()) {
		RebuildWindowChrome(*window);
	}
}

void SlateEditorUnit::AddPanel(const Editor::Slate::EditorPanelPointer& panel, RefPtr<EditorWindow> window) {

	if (panel == nullptr) {
		return;
	}

	//!< panelの所有はUnitが持つ. widgetはweak_ptrで参照するため, ここが唯一の所有者になる.
	//!< tabを閉じてもインスタンスは残るので, 後から同じpanelを開き直せる.
	if (std::find(panels_.begin(), panels_.end(), panel) == panels_.end()) {
		panels_.push_back(panel);
	}

	EditorWindow* target = (window != nullptr) ? window.Get() : GetMainEditorWindow().Get();

	if (target == nullptr) {
		STREAM_LOG_WARNING("SlateEditorUnit | AddPanel called before the main window is created.");
		return;
	}

	AddPanelToEditorWindow(*target, panel->GetDockPanel());
}

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class docking methods
////////////////////////////////////////////////////////////////////////////////////////////

void SlateEditorUnit::EnqueueTearOff(EditorWindow* source, Editor::Slate::DockPanelPointer panel, Vector2f clientPosition) {

	if (source == nullptr || panel == nullptr) {
		return;
	}

	//!< 同じpanelの要求が既にあるなら捨てる. 二重に処理すると同じpanelが2つのtreeへ入る.
	for (const TearOffRequest& queued : tearOffRequests_) {
		if (queued.panel == panel) {
			return;
		}
	}

	//!< Slateのイベントはclient座標なので, windowの配置に使うscreen座標へ変換する.
	POINT point = { static_cast<LONG>(clientPosition.x), static_cast<LONG>(clientPosition.y) };

	if (const HWND hwnd = source->viewport.GetWindow().GetHwnd(); hwnd != nullptr) {
		::ClientToScreen(hwnd, &point);
	}

	TearOffRequest request = {};
	request.source         = source;
	request.panel          = std::move(panel);
	request.screenPosition = { static_cast<float>(point.x), static_cast<float>(point.y) };

	tearOffRequests_.push_back(std::move(request));
}

void SlateEditorUnit::EnqueuePanelClose(EditorWindow* source, Editor::Slate::DockPanelPointer panel) {

	if (source == nullptr || panel == nullptr) {
		return;
	}

	for (const PanelCloseRequest& queued : panelCloseRequests_) {
		if (queued.panel == panel) {
			return;
		}
	}

	PanelCloseRequest request = {};
	request.source            = source;
	request.panel             = std::move(panel);

	panelCloseRequests_.push_back(std::move(request));
}

void SlateEditorUnit::ProcessPendingRequests() {

	//!< closeを先に処理する. 逆順にすると, 同じframeで × を押されたpanelが新しいwindowへ移った後に
	//!< 閉じられ, 生成直後のwindowが空のまま残る.
	if (!panelCloseRequests_.empty()) {
		//!< 処理中にwindowが増減するため, 要求は先に取り出しておく.
		std::vector<PanelCloseRequest> requests;
		requests.swap(panelCloseRequests_);

		for (const PanelCloseRequest& request : requests) {
			ProcessPanelCloseRequest(request);
		}
	}

	if (!tearOffRequests_.empty()) {
		std::vector<TearOffRequest> requests;
		requests.swap(tearOffRequests_);

		for (const TearOffRequest& request : requests) {
			ProcessTearOffRequest(request);
		}
	}
}

void SlateEditorUnit::ProcessTearOffRequest(const TearOffRequest& request) {

	//!< 要求を積んだwindowが既に破棄されている場合は何もしない.
	if (!IsAliveEditorWindow(request.source) || request.panel == nullptr) {
		return;
	}

	EditorWindow& source = *request.source;

	if (source.dockRoot == nullptr) {
		return;
	}

	//!< 元のtreeから外せない場合(他の要求で既に移動済み)は中止する.
	//!< ここを飛ばすと同じpanelが2つのtreeへ入る.
	if (!Editor::Slate::RemovePanelFromTree(source.dockRoot, request.panel)) {
		return;
	}

	//!< 空になったノードを詰める. PruneEmptyNodesは新しいrootを返す. (全て空ならnullptr)
	source.dockRoot = Editor::Slate::PruneEmptyNodes(source.dockRoot);
	RebuildWindowChrome(source);
	ApplyDockingHostToWindow(source);

	//!< 新しいsub windowを生成する. titleは引き出したpanel名にする.
	RefPtr<EditorWindow> torn = CreateEditorWindow(UnicodeConverter::ConvertW(request.panel->GetTitle()), kTearOffClientSize);

	if (torn == nullptr) {
		AddPanelToEditorWindow(source, request.panel); //!< 生成に失敗した場合は元のwindowへ戻す.
		return;
	}

	BuildLayout(*torn);                           //!< sub windowの受け皿(空のtab stack)を置く.
	AddPanelToEditorWindow(*torn, request.panel); //!< 引き出したpanelを入れ, hostを配り直す.

	//!< cursorの少し左上に置き, tab barを掴んだままdragを続けられるようにする.
	const Vector2i position = {
		static_cast<int32_t>(request.screenPosition.x) - kTearOffCursorOffsetX,
		static_cast<int32_t>(request.screenPosition.y) - static_cast<int32_t>(Editor::Slate::DockTabStack::TabBarHeight() * 0.5f)
	};

	MoveEditorWindow(*torn, position);

	CloseIfEmptyEditorWindow(source); //!< 全てのpanelを失ったsub windowは閉じる. (mainは残す)

	//!< buttonが押されたままならwindowのdragへ引き継ぐ. (掴んだまま他のwindowへ持っていける)
	if (SlateEditorUnit::IsVirtualKeyDown(VK_LBUTTON)) {
		BeginWindowDrag(*torn, GetCursorPosition());
	}
}

void SlateEditorUnit::ProcessPanelCloseRequest(const PanelCloseRequest& request) {

	if (!IsAliveEditorWindow(request.source) || request.panel == nullptr) {
		return;
	}

	EditorWindow& source = *request.source;

	if (source.dockRoot == nullptr) {
		return;
	}

	if (!Editor::Slate::RemovePanelFromTree(source.dockRoot, request.panel)) {
		return;
	}

	source.dockRoot = Editor::Slate::PruneEmptyNodes(source.dockRoot);
	RebuildWindowChrome(source);
	ApplyDockingHostToWindow(source);

	CloseIfEmptyEditorWindow(source);
}

void SlateEditorUnit::ApplyDockingHostToWindow(EditorWindow& window) {

	//!< 組み替えで生成されたDockTabStackにはhostが入っていないため, treeを触るたびに全体へ配り直す.
	//!< note: 走査はSplitterとDockTabStackしか辿らないため, chromeを含むrootではなくdockRootを渡す.
	Editor::Slate::ApplyDockingHost(window.dockRoot, &window);
}

void SlateEditorUnit::AddPanelToEditorWindow(EditorWindow& window, const Editor::Slate::DockPanelPointer& panel) {

	if (panel == nullptr) {
		return;
	}

	Editor::Slate::DockTabStackPointer stack = Editor::Slate::FindFirstTabStack(window.dockRoot);

	if (stack == nullptr) {
		//!< treeが空(全panelを失った)の場合は受け皿のtab stackを作る.
		Editor::Slate::Decl<Editor::Slate::DockTabStack> created;
		stack           = created.pointer;
		window.dockRoot = stack;
		RebuildWindowChrome(window);
	}

	stack->AddPanel(panel, true); //!< 裏に隠れると「出ない」ように見えるため前面にする.

	ApplyDockingHostToWindow(window);
}

void SlateEditorUnit::CloseIfEmptyEditorWindow(EditorWindow& window) {

	if (window.isMain) {
		return; //!< main windowは空でも閉じない. 閉じるとapplicationが終了するため土台として残す.
	}

	if (!Editor::Slate::CollectAllPanels(window.dockRoot).empty()) {
		return;
	}

	RequestCloseEditorWindow(&window);
}

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class window drag methods
////////////////////////////////////////////////////////////////////////////////////////////

void SlateEditorUnit::UpdateWindowDrag() {

	const Vector2i cursor      = GetCursorPosition();
	const bool     isLeftDown  = SlateEditorUnit::IsVirtualKeyDown(VK_LBUTTON);
	const bool     wasLeftDown = isPreviousLeftButtonDown_;

	isPreviousLeftButtonDown_ = isLeftDown;

	if (windowDrag_.window != nullptr && !IsAliveEditorWindow(windowDrag_.window)) {
		windowDrag_ = {}; //!< drag中のwindowが破棄された. (透過の解除は破棄で不要になる)
		ClearDockPreviews();
	}

	if (windowDrag_.window == nullptr) {

		//!< 押下のedgeでのみdragを開始する. 押したままの状態から拾わない.
		if (isLeftDown && !wasLeftDown) {
			TryBeginWindowDrag(cursor);
		}

		return;
	}

	if (!isLeftDown) {
		EndWindowDrag(cursor);
		return;
	}

	//!< windowをcursorへ追従させる.
	const Vector2i position = { cursor.x - windowDrag_.grabOffset.x, cursor.y - windowDrag_.grabOffset.y };
	MoveEditorWindow(*windowDrag_.window, position);

	//!< drop先の候補を更新する. (描画はRenderDockOverlayが行う)
	UpdateDockPreview(windowDrag_.window, cursor);
}

bool SlateEditorUnit::TryBeginWindowDrag(const Vector2i& cursor) {

	for (const EditorWindowPointer& window : windows_) {

		if (window->isMain) {
			continue; //!< main windowはOSのtitle barで移動する. (HTCAPTION)
		}

		if (!window->viewport.GetWindow().IsOpen() || window->dockRoot == nullptr) {
			continue;
		}

		Vector2f client = {};

		if (!ToClientPosition(window->viewport.GetWindow().GetHwnd(), cursor, client)) {
			continue;
		}

		//!< tab barの行の, tabが無い空きを掴んだときだけwindowのdragとみなす.
		//!< tabの上はDockTabStackがtabのdrag(= 切り離し)として処理する.
		if (client.y >= Editor::Slate::DockTabStack::TabBarHeight()) {
			continue;
		}

		if (Editor::Slate::IsPointOnAnyTab(window->dockRoot, client)) {
			continue;
		}

		BeginWindowDrag(*window, cursor);
		return true;
	}

	return false;
}

void SlateEditorUnit::BeginWindowDrag(EditorWindow& window, const Vector2i& cursor) {

	const HWND hwnd = window.viewport.GetWindow().GetHwnd();

	if (hwnd == nullptr) {
		return;
	}

	RECT rect = {};

	if (::GetWindowRect(hwnd, &rect) == FALSE) {
		return;
	}

	//!< 最大化されたwindowをdragし始めたら, まず通常サイズへ戻す.
	//!< 戻さないとSetWindowPos(SWP_NOSIZE)で最大化のサイズを保ったまま移動してしまい,
	//!< 画面いっぱいのwindowがcursorに張り付く. (OSのtitle bar dragと同じ挙動に揃える)
	if (::IsZoomed(hwnd) != FALSE) {

		//!< 戻す前にcursorが横幅のどのあたりを掴んでいたかを比率で覚えておく.
		const int32_t zoomedWidth = static_cast<int32_t>(rect.right - rect.left);

		const float ratioX = (zoomedWidth > 0)
			? static_cast<float>(cursor.x - static_cast<int32_t>(rect.left)) / static_cast<float>(zoomedWidth)
			: 0.5f;

		::ShowWindow(hwnd, SW_RESTORE);

		//!< 復元後の矩形で掴み位置を作り直す. 比率を保つとcursorの下から逃げない.
		if (::GetWindowRect(hwnd, &rect) == FALSE) {
			return;
		}

		const int32_t restoredWidth = static_cast<int32_t>(rect.right - rect.left);

		windowDrag_.window     = &window;
		windowDrag_.grabOffset = {
			static_cast<int32_t>(ratioX * static_cast<float>(restoredWidth)),
			static_cast<int32_t>(Editor::Slate::DockTabStack::TabBarHeight() * 0.5f)
		};

		//!< 復元でclient sizeが変わるため, このframeのうちにwindowをcursorへ寄せておく.
		MoveEditorWindow(window, { cursor.x - windowDrag_.grabOffset.x, cursor.y - windowDrag_.grabOffset.y });

		SlateEditorUnit::SetWindowDragOpacity(window, true);
		return;
	}

	windowDrag_.window     = &window;
	windowDrag_.grabOffset = { cursor.x - static_cast<int32_t>(rect.left), cursor.y - static_cast<int32_t>(rect.top) };

	SlateEditorUnit::SetWindowDragOpacity(window, true); //!< 下のマーカーが見えるように薄くする.
}

void SlateEditorUnit::EndWindowDrag(const Vector2i& cursor) {

	EditorWindow* dragged = windowDrag_.window;

	if (dragged != nullptr && IsAliveEditorWindow(dragged)) {
		SlateEditorUnit::SetWindowDragOpacity(*dragged, false); //!< 不透明に戻す.
	}

	//!< dropの瞬間のcursor位置でdrop先を取り直す.
	//!< UpdateDockPreviewはbuttonが押されているframeでしか走らないため, releaseのframeで
	//!< そのまま使うと1frame古い判定になり, マーカーの縁で離したときに取りこぼす.
	UpdateDockPreview(dragged, cursor);

	EditorWindow* target = FindEditorWindowUnderCursor(dragged, cursor);

	windowDrag_ = {}; //!< 以降のdrag状態は不要.

	if (dragged == nullptr || target == nullptr || dragged->dockRoot == nullptr || target->dockRoot == nullptr
		|| target->dockPreview.zone == Editor::Slate::DockZone::None) {

		ClearDockPreviews();
		return;
	}

	//!< 引き出し元のtreeを「そのまま」移す. panel一覧へ平坦化すると分割layoutが失われる.
	const Editor::Slate::WidgetPointer subtree = dragged->dockRoot;

	if (subtree == nullptr || Editor::Slate::CollectAllPanels(subtree).empty()) {
		ClearDockPreviews();
		return;
	}

	//!< 先に元windowのtreeを空にする. 同じsubtreeが2つのwindowに載ったままRenderEditorへ進むと,
	//!< 同じwidgetを2回layoutして矩形のcache(tabRects_)が壊れる.
	dragged->dockRoot = nullptr;
	RebuildWindowChrome(*dragged);

	//!< note: DockSubtreeIntoTargetはtarget.stackがtarget->dockRootの中に居ることが前提.
	//!<       別のtreeから取ったDockTargetを渡すとincomingが黙って消える.
	//!<       UpdateDockPreviewがtarget->dockRootから取っているためこの前提は満たされている.
	target->dockRoot = Editor::Slate::DockSubtreeIntoTarget(target->dockRoot, target->dockPreview, subtree);
	RebuildWindowChrome(*target);

	//!< 移ってきたtab stackのhostは引き出し元を指しているため, 必ずtarget側へ配り直す.
	ApplyDockingHostToWindow(*target);

	ClearDockPreviews();

	//!< 中身を移したwindowは閉じる. mainはapplicationが終了するため閉じない.
	if (!dragged->isMain) {
		RequestCloseEditorWindow(dragged);
	}
}

void SlateEditorUnit::UpdateDockPreview(EditorWindow* dragged, const Vector2i& cursor) {

	ClearDockPreviews();

	EditorWindow* target = FindEditorWindowUnderCursor(dragged, cursor);

	if (target == nullptr || target->dockRoot == nullptr) {
		return;
	}

	Vector2f client = {};

	if (!ToClientPosition(target->viewport.GetWindow().GetHwnd(), cursor, client)) {
		return;
	}

	const Editor::Slate::Geometry area = GetDockArea(*target);

	Editor::Slate::DockTarget preview = {};

	if (!Editor::Slate::FindDockTargetAt(target->dockRoot, area, client, preview)) {
		//!< tab stackが無い(空のwindow)場合は領域全体を対象にする.
		preview.stack = nullptr;
		preview.area  = area;
	}

	preview.zone = SlateEditorUnit::ResolveDockZone(preview, client);

	target->dockPreview = preview;
}

void SlateEditorUnit::ClearDockPreviews() {

	for (const EditorWindowPointer& window : windows_) {
		window->dockPreview = {};
	}
}

void SlateEditorUnit::MoveEditorWindow(EditorWindow& window, const Vector2i& position) {

	const HWND hwnd = window.viewport.GetWindow().GetHwnd();

	if (hwnd == nullptr) {
		return;
	}

	//!< SWP_NOSIZEでサイズを変えない. サイズを変えるとswap chainのresizeが走る.
	::SetWindowPos(hwnd, nullptr, position.x, position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

SlateEditorUnit::EditorWindow* SlateEditorUnit::FindEditorWindowUnderCursor(const EditorWindow* dragged, const Vector2i& cursor) {

	//!< 生成順の逆から探してz-orderを近似する.
	//!< note: dragged自身がcursorの真下に居るためWindowFromPointは使えない.
	for (auto it = windows_.rbegin(); it != windows_.rend(); ++it) {

		EditorWindow* window = it->get();

		if (window == dragged) {
			continue;
		}

		if (!window->viewport.GetWindow().IsOpen()) {
			continue;
		}

		if (!CanDockInto(dragged, window)) {
			continue;
		}

		if (ContainsScreenPoint(*window, cursor)) {
			return window;
		}
	}

	return nullptr;
}

Editor::Slate::DockZone SlateEditorUnit::ResolveDockZone(const Editor::Slate::DockTarget& target, const Vector2f& client) {

	using Editor::Slate::DockZone;

	const Editor::Slate::Geometry& area = target.area;

	if (area.localSize.x <= 0.0f || area.localSize.y <= 0.0f) {
		return DockZone::None;
	}

	if (!area.ContainsAbsolute(client)) {
		return DockZone::None; //!< 領域の外ではdropしない.
	}

	//!< 1) マーカーの上に居るならそれを最優先する. 意図が明確なため.
	//!<    note: 当たり判定は HitTestDockZoneMarkers 側で見た目より広げている.
	if (const DockZone zone = Editor::Slate::HitTestDockZoneMarkers(area, client); zone != DockZone::None) {
		return zone;
	}

	//!< 2) tab barの行に居るならtabとして追加する. (中央と同じ)
	//!<    「ドッキング済みのtabへ足す」がこのgestureで済むようにする.
	if (target.stack != nullptr) {

		const float tabBarBottom = area.absolutePosition.y + Editor::Slate::DockTabStack::TabBarHeight();

		if (client.y < tabBarBottom) {
			return DockZone::Center;
		}
	}

	//!< 3) マーカーを外していても, 領域内の位置からゾーンを推定する.
	//!<    マーカーだけを条件にすると狙いが厳しすぎて実用にならない.
	//!<    端から kDockEdgeRatio までが上下左右, それより内側は中央.
	//!<    note: 最も近い縁を採用する. 角では距離が小さい方が勝つ.
	const float left   = (client.x - area.absolutePosition.x) / area.localSize.x;
	const float top    = (client.y - area.absolutePosition.y) / area.localSize.y;
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

void SlateEditorUnit::SetWindowDragOpacity(EditorWindow& window, bool isDragging) {

	const HWND hwnd = window.viewport.GetWindow().GetHwnd();

	if (hwnd == nullptr) {
		return;
	}

	//!< drag中のwindowはcursorの下に居てdrop先のマーカーを覆い隠す.
	//!< 半透明にして下が見えるようにする. (UE5やVisual Studioと同じ見せ方)
	const LONG_PTR style = ::GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

	if (isDragging) {
		::SetWindowLongPtrW(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(hwnd, 0, kWindowDragAlpha, LWA_ALPHA);

	} else {
		//!< WS_EX_LAYEREDを外して元の描画に戻す. alphaを255に戻すだけでは合成が残る.
		::SetWindowLongPtrW(hwnd, GWL_EXSTYLE, style & ~WS_EX_LAYERED);
		::RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
	}
}

float SlateEditorUnit::GetChromeTopHeight(const EditorWindow& window) const {

	if (!window.isMain) {
		return 0.0f; //!< sub windowはOverlayで全面に敷いているためchromeは高さを取らない.
	}

	float height = Editor::Slate::TitleBar::Height();

	//!< menu barは設定されているときだけ積まれる. (RebuildWindowChromeと必ず一致させる)
	if (menuBar_ != nullptr) {
		const float menuBarHeight = menuBar_->GetHeight();
		height += (menuBarHeight > 0.0f) ? menuBarHeight : Editor::Slate::ImGuiMenuBar::Height();
	}

	return height;
}

Editor::Slate::Geometry SlateEditorUnit::GetDockArea(const EditorWindow& window) const {

	if (window.root == nullptr) {
		return {};
	}

	//!< dockRootが実際に置かれている矩形. chromeの分だけclientより小さい.
	//!< main windowはTitleBarと(あれば)MenuBarを縦に積んでいるためその高さだけ下がる.
	//!< sub windowはOverlayで全面に敷いているためclient全体と一致する.
	const Vector2f client = window.root->GetClientSize();

	const float chromeTop = GetChromeTopHeight(window);

	Editor::Slate::Geometry area = {};
	area.absolutePosition        = { 0.0f, chromeTop };
	area.localSize               = { client.x, client.y - chromeTop };
	area.scale                   = window.root->GetDpiScale();

	return area;
}

bool SlateEditorUnit::CanDockInto(const EditorWindow* dragged, const EditorWindow* target) {

	if (dragged == nullptr || target == nullptr) {
		return false;
	}

	//!< main windowをsub windowへ入れない. mainは土台なので移動先にはなれない.
	if (dragged->isMain && !target->isMain) {
		return false;
	}

	return true;
}

bool SlateEditorUnit::ContainsScreenPoint(const EditorWindow& window, const Vector2i& cursor) {

	RECT rect = {};

	if (::GetWindowRect(window.viewport.GetWindow().GetHwnd(), &rect) == FALSE) {
		return false;
	}

	return (cursor.x >= rect.left) && (cursor.x < rect.right) && (cursor.y >= rect.top) && (cursor.y < rect.bottom);
}

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class input methods
////////////////////////////////////////////////////////////////////////////////////////////

void SlateEditorUnit::UpdateInput(EditorWindow& window) {

	//!< note: Editor専用の入力取得としてWinAPIを直接pollingしている. Platform::Inputは使用しない.
	//!<       理由: Platform::InputのMouse/Keyboardは単一のCooperativeLevel windowを前提としており,
	//!<       Editorのようにwindowが動的に増減する構成ではhwndを切り替えるたびにAcquireが外れ,
	//!<       tab選択やwindowの最大化といった単発clickを取り落とすため.

	if (window.root == nullptr) {
		return;
	}

	const HWND hwnd = window.viewport.GetWindow().GetHwnd();

	if (hwnd == nullptr) {
		return;
	}

	//!< 前景のwindowでなければ入力を配送しない. 複数windowが同じmouse stateを共有してしまうため.
	const bool isForeground = (::GetForegroundWindow() == hwnd);

	{ //!< マウス位置をclient座標系で求める.
		POINT point = {};

		if (::GetCursorPos(&point) != FALSE && ::ScreenToClient(hwnd, &point) != FALSE) {
			window.mousePosition = { static_cast<float>(point.x), static_cast<float>(point.y) };
		}
	}

	window.renderer.InjectMousePosition(window.mousePosition);

	//!< TODO: マウスホイールはpollingでは取得できないため未対応. WM_MOUSEWHEELの受け口が必要.

	//!< 修飾キーの状態を組む.
	FlagEnum<Editor::Slate::PointerEvent::Modifier> modifier = Editor::Slate::PointerEvent::Modifier::None;

	if (SlateEditorUnit::IsVirtualKeyDown(VK_CONTROL)) {
		modifier.Set(Editor::Slate::PointerEvent::Modifier::Ctrl);
	}

	if (SlateEditorUnit::IsVirtualKeyDown(VK_SHIFT)) {
		modifier.Set(Editor::Slate::PointerEvent::Modifier::Shift);
	}

	if (SlateEditorUnit::IsVirtualKeyDown(VK_MENU)) {
		modifier.Set(Editor::Slate::PointerEvent::Modifier::Alt);
	}

	//!< 入力routingの決定.
	//!< 1. Slateがマウスをキャプチャ中(splitterのdrag等)なら, cursorがImGuiのpanel上へ移っても
	//!<    Slateへ送り続ける. そうしないとdragが途中で止まる.
	//!< 2. キャプチャしていない かつ ImGuiのwidgetが操作を握っているなら, Slateへは送らない.
	//!< 3. それ以外はSlateへ送る.
	//!< note: ImGuiIO::WantCaptureMouseは前frameのマウス位置を基に計算されるため1frame古く,
	//!<       dragが固まる原因になるので判定には使わない. IsInteractingはActiveIdを見る.
	const bool isWindowDragging = (windowDrag_.window == &window);
	const bool hasCapture       = window.application.HasMouseCapture();
	const bool isRouteToSlate   = !isWindowDragging && (hasCapture || (isForeground && !window.renderer.IsInteracting()));

	static constexpr int32_t kVirtualKeys[kMouseButtonCount] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON };

	static constexpr Editor::Slate::PointerEvent::Button kButtons[kMouseButtonCount] = {
		Editor::Slate::PointerEvent::Button::Left,
		Editor::Slate::PointerEvent::Button::Right,
		Editor::Slate::PointerEvent::Button::Middle
	};

	Editor::Slate::PointerEvent event = {};
	event.screenPosition = window.mousePosition;
	event.modifier       = modifier;

	//!< マウス移動は毎frame配送する. (hoverのEnter / Leaveとcursor形状の更新に必要)
	if (isRouteToSlate) {
		event.button = kButtons[0];
		window.application.ProcessMouseMove(*window.root, event);
	}

	for (size_t i = 0; i < kMouseButtonCount; ++i) {

		//!< 前景でない場合はedgeを取らない. 押下したままwindowを切り替えたときに誤検出するため.
		const bool isDown = isForeground && SlateEditorUnit::IsVirtualKeyDown(kVirtualKeys[i]);

		if (isDown == window.mouseButtons[i]) {
			continue; //!< 状態が変化していない場合は何もしない.
		}

		window.renderer.InjectMouseButton(static_cast<int32_t>(i), isDown);

		//!< buttonを離すeventはキャプチャの解放に必要なため, routingの判定に関わらず必ず送る.
		if (isRouteToSlate || !isDown) {
			event.button = kButtons[i];

			if (isDown) {
				window.application.ProcessMouseButtonDown(*window.root, event);

			} else {
				window.application.ProcessMouseButtonUp(*window.root, event);
			}
		}

		//!< routingの結果に関わらず状態は保存する. (保存しないとedge検出がずれる)
		window.mouseButtons[i] = isDown;
	}
}

bool SlateEditorUnit::IsVirtualKeyDown(int32_t key) {
	return (::GetAsyncKeyState(key) & 0x8000) != 0;
}
