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
#include <Runtime/Editor/Slate/Widgets/SlateDecl.h>
#include <Runtime/Editor/Slate/Widgets/SlateBoxPanel.h>
#include <Runtime/Editor/Slate/Widgets/SlateOverlay.h>
#include <Runtime/Editor/Slate/Widgets/SlateTitleBar.h>
#include <Runtime/Editor/Slate/Docking/SlateDockPanel.h>
#include <Runtime/Editor/Slate/Docking/SlateDockTabStack.h>
#include <Runtime/Editor/Slate/Docking/SlateDocking.h>
#include <Runtime/Editor/Slate/Docking/SlateSplitter.h>

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Time/TimePoint.h>

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
	//!< TODO: Configurationから取得できるようにする.
	static constexpr DXGI_FORMAT kBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

}

//-----------------------------------------------------------------------------------------
// helper
//-----------------------------------------------------------------------------------------

namespace {

	//!< UTF-8のpanel名をwindow名(UTF-16)へ変換する.
	std::wstring ConvertToWide(const std::string& text) {

		if (text.empty()) {
			return L"Panel";
		}

		const char* pointer  = text.data();
		const int32_t length = ::MultiByteToWideChar(CP_UTF8, 0, pointer, static_cast<int32_t>(text.size()), nullptr, 0);

		if (length <= 0) {
			return L"Panel";
		}

		std::wstring result(static_cast<size_t>(length), L'\0');
		::MultiByteToWideChar(CP_UTF8, 0, pointer, static_cast<int32_t>(text.size()), result.data(), length);

		return result;
	}

	//!< cursorのscreen座標を取得する.
	Vector2i GetCursorPosition() {

		POINT point = {};

		if (::GetCursorPos(&point) == FALSE) {
			return {};
		}

		return { static_cast<int32_t>(point.x), static_cast<int32_t>(point.y) };
	}

	//!< screen座標をclient座標へ変換する.
	//!< retval false client領域の外. (title barやリサイズ枠の上)
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
// SlateEditorUnit class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SlateEditorUnit::Setup(Framework::Pipeline& pipeline) {

	//!< window生成はGraphics::Coreの初期化後でなければならないため, Priority::Lowestで後に走らせる.
	pipeline.SetProcess(Framework::Phase::Initialize, Framework::Priority::Lowest, [this]() { InitEditor(); });

	//!< swap chainのresizeとinputのpolling.
	//!< note: resizeはPresentの完了後でなければならない. Phase::Updateは前frameのPresentより後なので満たしている.
	pipeline.SetProcess(Framework::Phase::Update, Framework::Priority::Normal, [this]() { UpdateEditor(); });

	//!< 全windowのlayoutと描画.
	pipeline.SetProcess(Framework::Phase::Render, Framework::Priority::Lowest, [this]() { RenderEditor(); });

	//!< 全windowのPresentと, 閉じられたwindowの回収.
	pipeline.SetProcess(Framework::Phase::EndFrame, Framework::Priority::Normal, [this]() { PresentEditor(); });

	//!< Phase::Terminateはpriorityの逆順で実行されるため, Priority::LowestがTerminateの最初に走る.
	pipeline.SetProcess(Framework::Phase::Terminate, Framework::Priority::Lowest, [this]() { TermEditor(); });

	//!< loopを抜ける条件. message pumpもここで回す.
	//!< note: WindowUnitも同じconditionを登録するため, 併用するとmessage pumpが二重に回る. どちらか一方のみ登録する.
	pipeline.SetCondition([this]() { return IsRequestExit(); });
}

void SlateEditorUnit::InitEditor() {

	//!< main windowを生成する. 以降のsub windowはdockingのtear-offから生成される.
	RefPtr<EditorWindow> window = CreateEditorWindow(L"[Sxavenger Engine] Slate Editor", kDefaultClientSize, true);

	if (window == nullptr) {
		StreamLogger::Assert(false, "SlateEditorUnit | failed to create main editor window.");
		return;
	}

	BuildLayout(*window);

	frameTracker_.Start(); //!< frame間の時間の計測を開始する.

	StreamLogger::Info("SlateEditorUnit | slate editor initialized.");
}

void SlateEditorUnit::UpdateEditor() {

	//!< 前frameの入力処理中に積まれた要求(tabの切り離し / tabを閉じる)をここでまとめて処理する.
	//!< 入力の配送より前に行う理由は, 差し替えが終わったtreeへ入力を流すため, および同じframeで
	//!< 生成したwindowにもresizeの追従と入力の配送を行き渡らせるため.
	//!< note: windows_はstd::listなので, 処理中にwindowが追加されても下のloopは壊れない.
	ProcessPendingRequests();

	for (const EditorWindowPointer& window : windows_) {

		if (!window->viewport.GetWindow().IsOpen()) {
			continue; //!< OSに閉じられたwindowは触らない. PresentEditorで回収する.
		}

		//!< SwapChain::Resizeはback bufferを解放するが, GPUの完了を待たない.
		//!< 待たずに解放すると EXECUTION ERROR #921 (OBJECT_DELETED_WHILE_STILL_IN_USE) になるため,
		//!< resizeが発生するフレームだけ, 先にqueueをflushしてGPUの完了を待つ.
		//!< note: SubmitDirectQueue -> ExecuteAll -> Reset -> WaitGpu でCPU側が待機する.
		if (window->viewport.GetWindow().GetEvent() == Platform::Window::Event::Resize) {
			Graphics::Core::SubmitDirectQueue();
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

	//!< frame間の時間を求める. RunTimeTrackerはmicrosecondで計測するため, secondへ変換する.
	frameTracker_.Stop();
	const double measured = frameTracker_.GetMeasuredTime().time;
	frameTracker_.Start();

	const TimePointf<TimeUnit::Second> deltaTime = TimePointf<TimeUnit::Second>(
		static_cast<float>(measured * TimeUtil::ConvertFactor<TimeUnit::Microsecond, TimeUnit::Second, double>())
	);
	//!< note: 0を渡してもImGuiRenderer::BeginFrame側でFLT_MINへclampされるため, ここでのclampは行わない.

	//!< Widget::PaintArguments::timeはアニメーションの位相に使うため, deltaTimeではなく起動からの累計時間を渡す.
	elapsedTime_ += deltaTime;

	for (const EditorWindowPointer& window : windows_) {

		if (!window->viewport.GetWindow().IsOpen() || window->root == nullptr) {
			continue;
		}

		RenderWindow(*window, deltaTime);
	}

	//!< 全windowのcommand listをまとめてGPUへ投入する.
	//!< note: 現状このprojectで描画を行うUnitはSlateEditorUnitのみで, Graphics::Core::SubmitDirectQueue()を
	//!<       呼ぶ処理は他に存在しない. 呼ばないとcommand listが実行されず空のback bufferがPresentされる.
	//!< TODO: 他のUnitがframeの投入を担うようになったら, この呼び出しを削除して二重実行を避ける.
	Graphics::Core::SubmitDirectQueue();
}

void SlateEditorUnit::RenderWindow(EditorWindow& window, TimePointf<TimeUnit::Second> deltaTime) {

	Graphics::GraphicsCommandContext& context = Graphics::Core::GetCommandContextDirect();
	Graphics::SwapChain::Buffer&      buffer  = window.viewport.GetCurrentBackBuffer();

	//!< back bufferをrender targetとして設定する.
	buffer.TransitionRenderTarget(context);
	//!< clearもRTV(sRGB)への書き込みなので, linearへ戻してから渡す.
	buffer.ClearRenderTarget(context, Editor::Slate::ConvertToLinearColor(Editor::Slate::Style::GetStyle().colors.background));
	buffer.OMSetRenderTarget(context);

	//!< Slateの描画. ImGuiのcontextはwindowごとに独立しているため, frameの開始 / 終了もwindowごとに行う.
	window.renderer.BeginFrame(window.root->GetClientSize(), deltaTime);
	window.application.Tick(*window.root, &window.renderer, elapsedTime_);

	//!< ドッキングのdrop先のoverlay. widgetより手前に出すため, Tickの後に最前面へ描く.
	SlateEditorUnit::RenderDockOverlay(window);

	window.renderer.EndFrame(context);

	buffer.TransitionPresent(context);
}

void SlateEditorUnit::RenderDockOverlay(EditorWindow& window) {

	const Editor::Slate::Geometry area = window.dockPreview.area;

	if (area.localSize.x <= 0.0f || area.localSize.y <= 0.0f) {
		return; //!< このwindowはdrop先になっていない.
	}

	const Editor::Slate::Style& style = Editor::Slate::Style::GetStyle();

	//!< ImGuiのwidgetより手前に出すため前景のDrawListへ描く. 描き終わったら元に戻す.
	window.renderer.SetDrawTarget(Editor::Slate::ImGuiRenderer::DrawTarget::Foreground);

	//!< 1. 対象のtab stack全体をうっすら示す. (どのpanelへ入るかが分かるように)
	window.renderer.DrawRect(area, style.colors.activeSoft);
	window.renderer.DrawBorder(area, style.colors.active, style.metrics.borderThin);

	//!< 2. dropした結果の領域. (マーカーの上にいるときだけ)
	if (window.dockPreview.zone != Editor::Slate::DockZone::None) {
		const Editor::Slate::Geometry preview = Editor::Slate::DockZonePreviewGeometry(area, window.dockPreview.zone);

		window.renderer.DrawRect(preview, style.colors.activeSoft);
		window.renderer.DrawBorder(preview, style.colors.active, style.metrics.borderThick);
	}

	//!< 3. ゾーンのマーカー. 描画と当たり判定でBuildDockZoneMarkers()を共有する.
	Editor::Slate::DockZoneMarker markers[Editor::Slate::kDockZoneMarkerCount] = {};
	Editor::Slate::BuildDockZoneMarkers(area, markers);

	for (const Editor::Slate::DockZoneMarker& marker : markers) {
		const bool isHot = (marker.zone == window.dockPreview.zone);

		window.renderer.DrawRect(marker.box, isHot ? style.colors.active : style.colors.panel);
		window.renderer.DrawBorder(marker.box, isHot ? style.colors.text : style.colors.border, style.metrics.borderThin);

		//!< 結合結果を示す最小限の図. 中央 = tab(枠のみ) / 上下左右 = 分割される側を塗る.
		//!< note: 移植元はcolors.whiteを使うが, Engineのstyleには無いのでcolors.textで代用する.
		const Color4f ink     = isHot ? style.colors.text : style.colors.textDim;
		const float   padding = marker.box.localSize.x * 0.22f;

		const Editor::Slate::Geometry inner = {
			{ marker.box.absolutePosition.x + padding, marker.box.absolutePosition.y + padding },
			{ marker.box.localSize.x - padding * 2.0f, marker.box.localSize.y - padding * 2.0f },
			area.scale
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

	CollectClosedWindows(); //!< Presentの後に回収する. (描画中のwindowを破棄しないため.)
}

void SlateEditorUnit::TermEditor() {

	closeRequests_.clear();
	tearOffRequests_.clear();
	panelCloseRequests_.clear();
	windowDrag_ = {};

	//!< 全windowのGPU resourceを解放する前に, GPUの完了を待つ. (理由はCollectClosedWindowsと同じ)
	if (!windows_.empty()) {
		Graphics::Core::SubmitDirectQueue();
	}

	for (const EditorWindowPointer& window : windows_) {
		window->root.reset();          //!< widget treeを先に破棄する.
		window->renderer.Shutdown();   //!< ImGuiのcontextはwidgetの後に破棄する.
		window->viewport.GetWindow().Reset(); //!< DestroyWindowとUnregisterClassを行う.
	}

	windows_.clear(); //!< ここでViewport(swap chain)が破棄される.
}

bool SlateEditorUnit::IsRequestExit() {

	//!< message pumpは全windowで共有されるため, 1frameに1度だけ回す.
	if (!Platform::Window::ProcessMessage()) {
		return true; //!< WM_QUITを受け取った. (main windowが破棄された.)
	}

	//!< 全てのeditor windowが失われた場合も終了する.
	return windows_.empty();
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

	//!< sub windowはtab行をClientのままにする.
	//!< HTCAPTIONを返すとOSのmodalなmove loopへ入り, その間Update / Renderが一切走らない.
	//!< drop先のoverlayを描けずドッキングの判定もできなくなるため, sub windowの移動はUnitが
	//!< SetWindowPosで自前に行う. (UpdateWindowDrag)
	if (!window.isMain) {
		return NonClientArea::Client;
	}

	if (window.root == nullptr) {
		return NonClientArea::Client;
	}

	const Vector2f position = { static_cast<float>(client.x), static_cast<float>(client.y) };

	//!< title barの行より下はwidgetが入力を受ける.
	if (position.y >= Editor::Slate::TitleBar::Height()) {
		return NonClientArea::Client;
	}

	//!< 最小化 / 最大化 / 閉じるボタンの上はwidgetに渡す. Captionにするとclickが押下判定にならない.
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
		//!< 現在の状態を見てトグルする. (TitleBar側はSetMaximizedで見た目だけを持つ)
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
		//!< main windowはTitleBarを縦に積む. OSのtitle barを出していないため, これが唯一のtitle barになる.
		Editor::Slate::Decl<Editor::Slate::VerticalBox> box;
		box->AddSlot().AutoSize().Content(CreateTitleBar(window, false));

		if (dock != nullptr) {
			box->AddSlot().Fill(1.0f).Content(dock);
		}

		window.root->SetPointer(box.pointer);
		return;
	}

	//!< sub windowはTitleを出さない. tab barの右上にボタンだけを重ね, その分だけtabの領域を空ける.
	//!< note: 余白は「右上のtab stack」にだけ入れる. 分割していると全stackに入れてしまうため,
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
// SlateEditorUnit class window methods
////////////////////////////////////////////////////////////////////////////////////////////

RefPtr<SlateEditorUnit::EditorWindow> SlateEditorUnit::CreateEditorWindow(const std::wstring& name, const Vector2ui& client, bool isMain) {

	if (isMain && GetMainEditorWindow() != nullptr) {
		StreamLogger::Warning("SlateEditorUnit | main editor window already exists.");
		return nullptr;
	}

	EditorWindowPointer window = std::make_unique<EditorWindow>();
	window->isMain = isMain;
	window->owner  = this; //!< 切り離し / 閉じる要求の実処理はUnitが行う.

	//!< Platform::Windowはwindow名からwindow classを作り, RegisterClassの失敗はexception(= 強制終了)になる.
	//!< classはwindowを閉じても解除されないため, 同じtitleのpanelを二度切り離すと名前が衝突して落ちる.
	//!< そのためclass名の元になる名前には一意な連番を付け, 表示上のtitleは生成後に戻す.
	const std::wstring identifier = name + L"##" + std::to_wstring(windowSerial_++);

	//!< main windowの破棄はWM_QUITを送るが, sub windowは送らない. dockingのtear-offはsub windowで生成する.
	window->viewport.Create(
		identifier, client,
		isMain ? Platform::Window::Category::Main : Platform::Window::Category::Sub,
		kBackBufferFormat,
		Platform::Window::Style::Borderless //!< OSのtitle barは出さず, Slate::TitleBarで描く.
	);

	//!< 表示上のtitleは連番を除いた名前に戻す. (window classは生成時の名前から決まるため影響しない)
	if (const HWND hwnd = window->viewport.GetWindow().GetHwnd(); hwnd != nullptr) {
		::SetWindowTextW(hwnd, name.c_str());
	}

	window->renderer.Init(); //!< ImGuiのcontextはwindowごとに1つ生成する.

	window->root = std::make_shared<Editor::Slate::Window>();
	SetupWindowChrome(*window); //!< OSのtitle barを消し, 自前のTitleBarで移動 / リサイズできるようにする.
	window->root->SetClientSize(static_cast<Vector2f>(window->viewport.GetClient()));

	windows_.push_back(std::move(window));

	StreamLogger::Info(L"SlateEditorUnit | create editor window. name: {}", name);

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

	//!< swap chainのback bufferとImGuiのGPU resourceを解放する前に, GPUの完了を待つ.
	//!< 直前のPresentがback bufferを参照したままなので, 待たずに解放すると
	//!< EXECUTION ERROR #921 (OBJECT_DELETED_WHILE_STILL_IN_USE) になる.
	//!< note: SubmitDirectQueue -> ExecuteAll -> Reset -> WaitGpu でCPU側が待機する.
	//!<       Presentの後にSignalを積んで待つため, Presentがqueueへ積んだ処理も完了が保証される.
	Graphics::Core::SubmitDirectQueue();

	for (EditorWindow* request : closeRequests_) {

		const auto it = std::find_if(windows_.begin(), windows_.end(), [request](const EditorWindowPointer& window) {
			return window.get() == request;
		});

		if (it == windows_.end()) {
			continue; //!< すでに回収済み. (二重に予約された場合.)
		}

		//!< このwindowを指す遅延要求とdrag状態を捨てる. 残すとerase後にdangling pointerを触る.
		DiscardRequests(request);

		(*it)->root.reset();
		(*it)->renderer.Shutdown();
		//!< Viewport::CloseはCloseWindow(= 最小化)を呼ぶだけでwindowを破棄しない.
		//!< 破棄しないとGWLP_USERDATAが解放済みのWindow*を指したままmessageを受け取り, use after freeになる.
		//!< Window::ResetはDestroyWindowとUnregisterClassを行うため, こちらを使う.
		//!< note: UnregisterClassも行われるので, 同じtitleのpanelを再度切り離してもclass名が衝突しない.
		(*it)->viewport.GetWindow().Reset();

		windows_.erase(it);
	}

	closeRequests_.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class layout methods
////////////////////////////////////////////////////////////////////////////////////////////

void SlateEditorUnit::BuildLayout(EditorWindow& window) {

	if (window.root == nullptr) {
		return;
	}

	if (!window.isMain) {
		//!< sub windowはtear-offされたpanelの受け皿として空のtab stackを置く.
		Editor::Slate::Decl<Editor::Slate::DockTabStack> stack;

		window.dockRoot = stack.pointer;
		RebuildWindowChrome(window);
		ApplyDockingHostToWindow(window);
		return;
	}

	//!< main windowの最小構成のlayout. splitterで左右に分割し, それぞれにtab stackを置く.
	//!< note: 各DockPanelのcontentは未設定. CompoundWidgetがnullptrを許容するため描画は行われない.

	Editor::Slate::Decl<Editor::Slate::DockTabStack> left;
	{
		Editor::Slate::Decl<Editor::Slate::DockPanel> panel;
		//!< iconはtitleの一部として埋め込む. 位置も個数も自由に決められる.
		panel->Title(std::format("{} Viewport", Editor::Slate::Icon::Videocam));
		panel->Closable(false); //!< 固定tabとして扱う.
		left->AddPanel(panel.pointer);
	}
	{
		Editor::Slate::Decl<Editor::Slate::DockPanel> panel;
		panel->Title(std::format("{} Content Browser", Editor::Slate::Icon::Folder));
		left->AddPanel(panel.pointer);
	}

	Editor::Slate::Decl<Editor::Slate::DockTabStack> right;
	{
		Editor::Slate::Decl<Editor::Slate::DockPanel> panel;
		panel->Title(std::format("{} Outliner", Editor::Slate::Icon::Hierarchy));
		right->AddPanel(panel.pointer);
	}
	{
		Editor::Slate::Decl<Editor::Slate::DockPanel> panel;
		panel->Title(std::format("{} Details", Editor::Slate::Icon::Settings));
		right->AddPanel(panel.pointer);
	}

	Editor::Slate::Decl<Editor::Slate::Splitter> splitter;
	splitter->SetOrientation(Editor::Slate::Splitter::Orientation::Horizontal);
	splitter->SetChildren(left.pointer, right.pointer);
	splitter->SetRatio(0.7f);

	window.dockRoot = splitter.pointer;
	RebuildWindowChrome(window); //!< TitleBarで包んでrootへ設定する.

	ApplyDockingHostToWindow(window); //!< 生成した全tab stackへ切り離し要求の通知先を配る.
}

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit::EditorWindow structure methods
////////////////////////////////////////////////////////////////////////////////////////////

bool SlateEditorUnit::EditorWindow::CanTearOff(const Editor::Slate::DockPanelPointer& panel) const {

	if (panel == nullptr || root == nullptr) {
		return false;
	}

	//!< main windowは空になっても閉じないため, 常に引き出してよい.
	if (isMain) {
		return true;
	}

	//!< sub windowの最後の1枚は引き出させない. 引き出すと元のwindowが空になって閉じられ,
	//!< 閉じる途中に同じwindowへ戻す操作ができてしまい, tabごと破棄されて消えるため.
	//!< (windowごと動かせばよい. UE5も単独tabは引き出せない)
	return Editor::Slate::CollectAllPanels(dockRoot).size() > 1;
}

void SlateEditorUnit::EditorWindow::RequestTearOff(Editor::Slate::DockPanelPointer panel, Vector2f screenPosition) {

	if (owner == nullptr) {
		return;
	}

	//!< 要求を積むだけに留める. この関数はDockTabStack::OnMouseMoveの中から呼ばれるため,
	//!< ここでwindowを生成したりtreeを差し替えると, 走査中のtreeを壊す.
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

	TearOffRequest request = {};
	request.source         = source;
	request.panel          = std::move(panel);
	request.screenPosition = clientPosition;

	//!< PointerEvent::screenPositionはclient座標系. (UpdateInputがScreenToClient済みの値を入れている)
	//!< 新しいwindowを置く位置はscreen座標で必要なので, windowが動く前にここで変換して持つ.
	POINT point = { static_cast<LONG>(clientPosition.x), static_cast<LONG>(clientPosition.y) };

	if (::ClientToScreen(source->viewport.GetWindow().GetHwnd(), &point) != FALSE) {
		request.screenPosition = { static_cast<float>(point.x), static_cast<float>(point.y) };
	}

	tearOffRequests_.push_back(std::move(request));
}

void SlateEditorUnit::EnqueuePanelClose(EditorWindow* source, Editor::Slate::DockPanelPointer panel) {

	if (source == nullptr || panel == nullptr) {
		return;
	}

	for (const PanelCloseRequest& queued : panelCloseRequests_) {
		if (queued.panel == panel) {
			return; //!< 同じpanelの要求が既にある.
		}
	}

	PanelCloseRequest request = {};
	request.source = source;
	request.panel  = std::move(panel);

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

	if (source.root == nullptr) {
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

	//!< 新しいsub windowを生成する. windowのtitleは引き出したpanel名にする. (UE5と同じ)
	RefPtr<EditorWindow> torn = CreateEditorWindow(ConvertToWide(request.panel->GetTitle()), kTearOffClientSize);

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

	SlateEditorUnit::MoveEditorWindow(*torn, position);

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

	if (source.root == nullptr) {
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

void SlateEditorUnit::DiscardRequests(const EditorWindow* window) {

	if (window == nullptr) {
		return;
	}

	std::erase_if(tearOffRequests_,    [window](const TearOffRequest& request) { return request.source == window; });
	std::erase_if(panelCloseRequests_, [window](const PanelCloseRequest& request) { return request.source == window; });

	if (windowDrag_.window == window) {
		windowDrag_ = {};
		ClearDockPreviews(); //!< 表示したままだとoverlayが残り続ける.
	}
}

void SlateEditorUnit::ApplyDockingHostToWindow(EditorWindow& window) {

	if (window.root == nullptr) {
		return;
	}

	//!< 組み替えで生成されたDockTabStackにはhostが入っていないため, treeを触るたびに全体へ配り直す.
	Editor::Slate::ApplyDockingHost(window.dockRoot, &window);

	//!< note: ResetTabBarInsets()は呼ばない. tab barの右余白はwindow操作ボタンを避けるためのもので,
	//!<       このUnitはまだcustom title barを載せておらず, 余白を設定する箇所が存在しないため.
	//!<       (title barを配線する際, そのlayout構築でreset -> 右上のstackへ再設定を行う)
}

void SlateEditorUnit::AddPanelToEditorWindow(EditorWindow& window, const Editor::Slate::DockPanelPointer& panel) {

	if (window.root == nullptr || panel == nullptr) {
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
		return; //!< main windowは空でも閉じない. 閉じるとapplicationが終了するため, 土台として残す.
	}

	if (!Editor::Slate::CollectAllPanels(window.dockRoot).empty()) {
		return;
	}

	RequestCloseEditorWindow(&window);
}

bool SlateEditorUnit::IsAliveEditorWindow(const EditorWindow* window) const {

	if (window == nullptr) {
		return false;
	}

	return std::find_if(windows_.begin(), windows_.end(), [window](const EditorWindowPointer& element) {
		return element.get() == window;
	}) != windows_.end();
}

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class window drag methods
////////////////////////////////////////////////////////////////////////////////////////////

void SlateEditorUnit::UpdateWindowDrag() {

	//!< dragの判定はUnitが自前でpollingする. EditorWindow::mouseButtonsはUpdateInputが
	//!< 更新済みでedgeが取れないため, ここで前frameの状態を持つ.
	//!< note: drag中はcursorが掴んでいるwindowの外(= drop先のwindowの上)へ出るが, UpdateInputの
	//!<       「前景windowでなければ配送しない」判定とは衝突しない. 掴んでいるwindowは押下時に
	//!<       activateされて前景のままであり, drop先のwindowへSlateの入力を送る必要も無いため.
	//!<       (drop先はUnitがtreeから直接求める. 前景でないwindowへ送ると, 掴んでいない側のtabが
	//!<        activeになる等の副作用が出る)
	const bool isDown  = SlateEditorUnit::IsVirtualKeyDown(VK_LBUTTON);
	const bool wasDown = isPreviousLeftButtonDown_;

	isPreviousLeftButtonDown_ = isDown;

	const Vector2i cursor = GetCursorPosition();

	if (windowDrag_.window != nullptr) {

		if (!isDown) {
			EndWindowDrag(cursor);
			return;
		}

		//!< cursorへ追従させ, drop先を更新する.
		//!< note: ここでwindowが移動してもUpdateInputはGetCursorPosから毎frame座標を取り直すため,
		//!<       client座標のずれは残らない.
		const Vector2i position = { cursor.x - windowDrag_.grabOffset.x, cursor.y - windowDrag_.grabOffset.y };

		SlateEditorUnit::MoveEditorWindow(*windowDrag_.window, position);
		UpdateDockPreview(cursor);
		return;
	}

	//!< 押した瞬間だけ判定する. 押したまま外から入ってきた場合にdragを始めないため.
	if (isDown && !wasDown) {
		TryBeginWindowDrag(cursor);
	}
}

bool SlateEditorUnit::TryBeginWindowDrag(const Vector2i& cursor) {

	//!< 手前にあるものを優先したいので後ろから探す.
	for (auto itr = windows_.rbegin(); itr != windows_.rend(); ++itr) {
		EditorWindow& window = **itr;

		//!< main windowはドッキングの受け入れ側に固定する. 他のwindowへ入れるとmainが空になり
		//!< 全panelがsubへ移るため, 掴ませない. (CanDockInto()と同じ方針)
		if (window.isMain || window.root == nullptr) {
			continue;
		}

		const HWND hwnd = window.viewport.GetWindow().GetHwnd();

		if (hwnd == nullptr || !window.viewport.GetWindow().IsOpen()) {
			continue;
		}

		//!< 実際にcursorの下にあるwindowかをOSのz-orderで判定する.
		if (::WindowFromPoint(POINT{ static_cast<LONG>(cursor.x), static_cast<LONG>(cursor.y) }) != hwnd) {
			continue;
		}

		Vector2f client = {};

		if (!ToClientPosition(hwnd, cursor, client)) {
			continue; //!< OSのtitle barやリサイズ枠の上. OSの処理に任せる.
		}

		//!< tab barの行の, tabが無い空きを掴んだときだけwindowのdragとみなす.
		//!< tabの上はDockTabStackがtabのdrag(= 切り離し)として処理する.
		if (client.y >= Editor::Slate::DockTabStack::TabBarHeight()) {
			continue;
		}

		if (Editor::Slate::IsPointOnAnyTab(window.dockRoot, client)) {
			continue;
		}

		BeginWindowDrag(window, cursor);
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

	windowDrag_.window     = &window;
	windowDrag_.grabOffset = { cursor.x - static_cast<int32_t>(rect.left), cursor.y - static_cast<int32_t>(rect.top) };

	//!< 切り離し直後にここへ来る場合, buttonは押されたままなので押下のedgeとして扱わない.
	isPreviousLeftButtonDown_ = true;
}

void SlateEditorUnit::UpdateDockPreview(const Vector2i& cursor) {

	EditorWindow* target = FindEditorWindowUnderCursor(windowDrag_.window, cursor);

	//!< 先に対象を決めてから他を消す. 「全消し -> 再設定」の順にすると, 内容が同じでも
	//!< 毎frame変化ありに見えてしまう.
	for (const EditorWindowPointer& window : windows_) {
		if (window.get() != target) {
			window->dockPreview = {};
		}
	}

	if (target == nullptr) {
		return;
	}

	const HWND hwnd = target->viewport.GetWindow().GetHwnd();

	Vector2f client = {};

	if (target->root == nullptr || !ToClientPosition(hwnd, cursor, client)) {
		target->dockPreview = {};
		return;
	}

	//!< dock領域はdockRootが実際に置かれている矩形. chromeの分だけclientより小さい.
	//!< main windowはTitleBarを縦に積んでいるためその高さだけ下がる. sub windowはOverlayで
	//!< 全面に敷いているためclient全体と一致する.
	//!< note: ここがdockRootの実際の配置とずれると, マーカーの描画位置と当たり判定がずれる.
	const Vector2f clientSize = target->root->GetClientSize();
	const float    chromeTop  = target->isMain ? Editor::Slate::TitleBar::Height() : 0.0f;

	const Editor::Slate::Geometry area = {
		{ 0.0f, chromeTop },
		{ clientSize.x, clientSize.y - chromeTop },
		target->root->GetDpiScale()
	};

	Editor::Slate::DockTarget preview = {};

	if (!Editor::Slate::FindDockTargetAt(target->dockRoot, area, client, preview)) {
		//!< tab stackが無い(空のwindow)場合は領域全体を対象にする.
		preview.stack = nullptr;
		preview.area  = area;
	}

	//!< 判定は必ずマーカーの当たり判定で行う. 描画位置と一致させるため.
	preview.zone = Editor::Slate::HitTestDockZoneMarkers(preview.area, client);

	target->dockPreview = preview;
}

void SlateEditorUnit::EndWindowDrag(const Vector2i& cursor) {

	EditorWindow* dragged = windowDrag_.window;

	if (dragged == nullptr) {
		ClearDockPreviews();
		return;
	}

	//!< 表示に使ったdockPreviewをそのままdropの判定に使う. (見た目と結果を必ず一致させる)
	UpdateDockPreview(cursor);

	EditorWindow* target = FindEditorWindowUnderCursor(dragged, cursor);

	windowDrag_ = {}; //!< 以降のdrag状態は不要.

	if (target != nullptr && target->root != nullptr && dragged->root != nullptr
		&& target->dockPreview.zone != Editor::Slate::DockZone::None) {

		//!< 引き出し元のtreeを「そのまま」移す. panel一覧へ平坦化すると分割layoutが失われるため.
		const Editor::Slate::WidgetPointer subtree = dragged->dockRoot;

		if (subtree != nullptr && !Editor::Slate::CollectAllPanels(subtree).empty()) {

			//!< 先に元windowのtreeを空にする. 同じsubtreeが2つのwindowに載ったままRenderEditorへ進むと,
			//!< 同じwidgetを2回layoutして矩形のcache(tabRects_)が壊れる.
			dragged->dockRoot = nullptr;
			RebuildWindowChrome(*dragged);

			target->dockRoot = Editor::Slate::DockSubtreeIntoTarget(target->dockRoot, target->dockPreview, subtree);
			RebuildWindowChrome(*target);

			//!< 移ってきたtab stackのhostは引き出し元を指しているため, 必ずtarget側へ配り直す.
			ApplyDockingHostToWindow(*target);

			//!< 中身を移したwindowは閉じる. mainはapplicationが終了するため閉じない.
			//!< note: CanDockInto()によりmainが移動元になるのは移動先もmainのときだけ(= 現状ありえない).
			if (!dragged->isMain) {
				RequestCloseEditorWindow(dragged);
			}
		}
	}

	ClearDockPreviews();
}

void SlateEditorUnit::ClearDockPreviews() {

	for (const EditorWindowPointer& window : windows_) {
		window->dockPreview = {};
	}
}

void SlateEditorUnit::MoveEditorWindow(EditorWindow& window, const Vector2i& screenPosition) {

	const HWND hwnd = window.viewport.GetWindow().GetHwnd();

	if (hwnd == nullptr) {
		return;
	}

	//!< sizeは変えない. 変えるとswap chainのresizeが走り, GPUの完了待ちが入る.
	::SetWindowPos(
		hwnd, nullptr, screenPosition.x, screenPosition.y, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE
	);
}

SlateEditorUnit::EditorWindow* SlateEditorUnit::FindEditorWindowUnderCursor(const EditorWindow* dragged, const Vector2i& cursor) const {

	//!< 手前にあるものを優先したいので後ろから探す.
	for (auto itr = windows_.rbegin(); itr != windows_.rend(); ++itr) {
		EditorWindow* window = itr->get();

		if (window == dragged || window->viewport.GetWindow().IsOpen() == false) {
			continue;
		}

		//!< 閉じる予定のwindowは対象外. ここへドッキングするとpanelごと破棄されて消える.
		if (std::find(closeRequests_.begin(), closeRequests_.end(), window) != closeRequests_.end()) {
			continue;
		}

		if (dragged != nullptr && !SlateEditorUnit::CanDockInto(*dragged, *window)) {
			continue;
		}

		if (!SlateEditorUnit::ContainsScreenPoint(*window, cursor)) {
			continue;
		}

		return window;
	}

	return nullptr;
}

bool SlateEditorUnit::CanDockInto(const EditorWindow& dragged, const EditorWindow& target) {

	//!< main windowをsub windowへ入れることは禁止する. 許すとmainが空になったうえで全panelが
	//!< floating側へ移り, 「閉じてもapplicationが終わらないwindowに全部入っている / mainは空のまま
	//!< 残る」壊れた状態になる. main windowは常に受け入れ側でいてほしい.
	if (dragged.isMain && !target.isMain) {
		return false;
	}

	return true;
}

bool SlateEditorUnit::ContainsScreenPoint(const EditorWindow& window, const Vector2i& cursor) {

	const HWND hwnd = window.viewport.GetWindow().GetHwnd();

	if (hwnd == nullptr || ::IsWindowVisible(hwnd) == FALSE || ::IsIconic(hwnd) != FALSE) {
		return false;
	}

	RECT rect = {};

	if (::GetWindowRect(hwnd, &rect) == FALSE) {
		return false;
	}

	const POINT point = { static_cast<LONG>(cursor.x), static_cast<LONG>(cursor.y) };

	return ::PtInRect(&rect, point) != FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SlateEditorUnit class input methods
////////////////////////////////////////////////////////////////////////////////////////////

void SlateEditorUnit::UpdateInput(EditorWindow& window) {

	//!< TODO: Platform::InputSystemはEngineUnitのprivate memberでgetterが無いため, WinAPIを直接pollingしている.
	//!<       InputSystemが公開されたら Platform::Mouse へ移行する.

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
	//!< 1. Slateがマウスをキャプチャ中(splitterのdrag等)なら, cursorがImGuiのpanel上へ移ってもSlateへ送り続ける.
	//!<    そうしないとdragが途中で止まる.
	//!< 2. キャプチャしていない かつ ImGuiのwidgetが操作を握っているなら, Slateへは送らない.
	//!< 3. Unitがこのwindowを移動中(tab barの空きを掴んでのdrag)なら, Slateへは送らない.
	//!<    送ると掴んだままのtab bar上でtabのdragが始まり, 切り離しが二重に走る.
	//!<    ただしbuttonを離すeventは下のloopで従来どおり送る. (キャプチャの解放に必要)
	//!< 4. それ以外はSlateへ送る.
	//!< note: ImGuiIO::WantCaptureMouseは前frameのマウス位置を基に計算されるため1frame古く,
	//!<       dragが固まる原因になるので判定には使わない. (ImGuiRenderer::IsInteracting()も同じ理由でActiveIdを見る)
	//!< TODO: ImGuiRenderer::IsInteracting()はframe外(isActiveFrame_ == false)では必ずfalseを返す.
	//!<       このUnitはPhase::Update, つまりBeginFrame/EndFrameの外でpollingするため, 現状は常に4.に落ちる.
	//!<       ImGuiのwidgetを優先するにはframeに依存しない問い合わせをImGuiRendererへ追加する必要がある.
	const bool hasCapture       = window.application.HasMouseCapture();
	const bool isWindowDragging = (windowDrag_.window == &window);
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

	//!< マウス移動は毎frame配送する. (hoverのEnter / Leaveとcursor形状の更新に必要.)
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

		//!< routingの結果に関わらず状態は保存する. (保存しないとedge検出がずれる.)
		window.mouseButtons[i] = isDown;
	}
}

bool SlateEditorUnit::IsVirtualKeyDown(int32_t key) {
	return (::GetAsyncKeyState(key) & 0x8000) != 0;
}
