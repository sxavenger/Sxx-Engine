#include "Window.h"
SXAVENGER_ENGINE_USING_(Platform)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Math/VectorComparison.h>
#include <Lib/Pointer/ReferencePointer.h>

//* windows
#include <windowsx.h> //!< GET_X_LPARAM / GET_Y_LPARAM

////////////////////////////////////////////////////////////////////////////////////////////
// Window class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Window::Create(
	const std::wstring& name, const Vector2ui& client, Category category,
	const FlagEnum<Style>& style) {

	//!< windowクラス名の設定
	name_ = name;
	std::wstring className = Window::GetWindowClassName(name_);

	//!< instanceの生成
	hinst_ = GetModuleHandle(nullptr);

	//!< categoryの設定
	category_ = category;

	//!< window設定
	WNDCLASS wc = {};
	wc.lpszClassName = className.c_str();
	wc.hInstance     = hinst_;
	wc.lpfnWndProc   = Window::GetWindowProcFunction(category_);
	StreamLogger::Assert(RegisterClass(&wc), "window register class failed.");

	//!< rectの設定(windowサイズの調整用)
	RECT rect = Window::ConvertClientRect(client);

	//!< client用のrectを保存する
	rect_ = rect;

	//!< windowスタイルの設定
	style_ = style;

	//!< borderlessの判定
	//!< Titlebar(WS_CAPTION)を持たないtop level windowをborderlessとして扱う.
	//!< Popup(全画面)とChildは非クライアント領域を持たないため対象外とし, 既存の挙動を維持する.
	isBorderless_ = !style_.Test(Style::Titlebar) && !style_.Any(Style::Popup) && !style_.Any(Style::Child);

	//!< ウィンドウサイズの調整
	if (!isBorderless_) {
		//!< borderlessはWM_NCCALCSIZEで非クライアント領域を潰すため, window sizeとclient sizeが一致する.
		//!< そのため調整を行わない.
		AdjustWindowRect(&rect, static_cast<DWORD>(style_), false);
	}
	Vector2ui size = Window::ConvertClientSize(rect);

	//!< windowの生成
	hwnd_ = CreateWindow(
		wc.lpszClassName,
		name.c_str(),
		static_cast<DWORD>(style_),
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		size.x,
		size.y,
		nullptr,
		nullptr,
		hinst_,
		this
	);
	StreamLogger::Assert(hwnd_ != nullptr, "window create failed.");

	if (isBorderless_) {
		//!< frameを再計算させ, WM_NCCALCSIZEの結果を反映させる.
		//!< 生成中の最初のWM_NCCALCSIZEはWM_CREATE(instanceの関連付け)より前に届くため処理できない.
		SetWindowPos(
			hwnd_, nullptr, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE
		);
	}

	StreamLogger::Info(
		L"Platform::Window | window create succeeded. name: {}, hwnd: {:p}", name, static_cast<const void*>(hwnd_)
	);
}

void Window::Reset() {
	if (hwnd_ == nullptr && hinst_ == nullptr) {
		return; //!< windowが生成されていない場合はリセット処理を行わない
	}

	if (hwnd_ != nullptr) {
		DestroyWindow(hwnd_);
		CloseWindow(hwnd_);
	}

	if (hinst_ != nullptr) {
		UnregisterClass(Window::GetWindowClassName(name_).c_str(), hinst_);
	}

	StreamLogger::Info(
		L"Platform::Window | window reset succeeded. name: {}", name_
	);

	//!< windowの情報をリセット
	hwnd_  = nullptr;
	hinst_ = nullptr;
	name_.clear();
}

void Window::Show(DWORD flag) const {
	ShowWindow(hwnd_, flag);
}

void Window::Open() const {
	ShowWindow(hwnd_, SW_SHOW);
}

void Window::Close() {
	if (hwnd_ == nullptr) {
		return;
	}

	CloseWindow(hwnd_);

	StreamLogger::Info(
		L"Platform::Window | window close. name: {}, hwnd: {:p}", name_, static_cast<const void*>(hwnd_)
	);
}

void Window::SetWindowIcon(const std::filesystem::path& filepath, const Vector2ui& size) const {
	HICON icon
		= static_cast<HICON>(LoadImageA(GetModuleHandle(NULL), filepath.generic_string().c_str(), IMAGE_ICON, size.x, size.y, LR_LOADFROMFILE));

	SendMessage(hwnd_, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
}

void Window::SetTaskbarIcon(const std::filesystem::path& filepath, const Vector2ui& size) const {
	HICON icon
		= static_cast<HICON>(LoadImageA(GetModuleHandle(NULL), filepath.generic_string().c_str(), IMAGE_ICON, size.x, size.y, LR_LOADFROMFILE));

	SendMessage(hwnd_, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
}

void Window::SetIcon(const std::filesystem::path& filepath, const Vector2ui& size) const {
	Window::SetWindowIcon(filepath, size);
	Window::SetTaskbarIcon(filepath, size);
}

bool Window::IsOpen() const {
	return hwnd_ != nullptr && IsWindow(hwnd_);
}

bool Window::ProcessMessage() {
	MSG msg = {};

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.message != WM_QUIT;
}

LRESULT Window::WindowProcMain(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	//!< windowのインスタンスを取得
	RefPtr<Window> window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	Message message = Window::GetMessageCode(msg); //!< WindowsのメッセージコードをWindow::Messageに変換する

	if (window != nullptr) {
		//!< Resizeは消費されるまで保持する. 毎messageでNoneに戻すと, resizeの直後に来る他のmessageで
		//!< 消えてしまい, swap chainがwindowのサイズに追従できない. (Window::ConsumeEventで消費する)
		if (window->event_ != Event::Resize) {
			window->event_ = Event::None; //!< eventを初期化する
		}
	}

	//if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
	//	return true;
	//}

	//!< borderless windowの非クライアント領域処理 (borderlessでない場合は何も行わない)
	if (window != nullptr) {
		LRESULT result = 0;
		if (window->ProcessBorderlessMessage(hwnd, message, wparam, lparam, result)) {
			return result;
		}
	}

	switch (message) {
		case Message::Create:
			{ //!< windowが生成された場合, instanceをウィンドウに関連付ける
				CREATESTRUCT* structure = reinterpret_cast<CREATESTRUCT*>(lparam);
				LONG_PTR instance       = reinterpret_cast<LONG_PTR>(structure->lpCreateParams);
				SetWindowLongPtr(hwnd, GWLP_USERDATA, instance);
			}
			break;

		case Message::Destroy:
			{ //!< windowが破棄された場合, OSに対してアプリの終了を伝える
				PostQuitMessage(0);
				return 0;
			}

		case Message::EnterSizeMove:
			{ //!< サイズ変更や移動のmodal loopに入った場合, loop中のWM_SIZEを無視するようにする
				if (window != nullptr) {
					window->isSizeMoving_ = true;
				}
			}
			break;

		case Message::ExitSizeMove:
			{ //!< windowのサイズ変更や移動が終了した場合, instanceにリサイズ処理を行う
				if (window != nullptr) {
					window->isSizeMoving_ = false;
					window->UpdateRect(Window::GetCurrentClientRect(hwnd)); //!< rectを更新する
				}
			}
			break;

		case Message::Size:
			{ //!< 最大化 / スナップ / SetWindowPosはmodal loopを通らずWM_EXITSIZEMOVEが来ないため,
			  //!< ここでrectを更新する. これが無いとGetClient()が古い値を返し, swap chainが追従しない.
				if (window == nullptr || window->isSizeMoving_) {
					break; //!< drag中はloopの終了時にまとめて処理する
				}

				if (wparam == SIZE_MINIMIZED) {
					break; //!< 最小化ではclient領域が0になるためresizeしない
				}

				window->UpdateRect(Window::GetCurrentClientRect(hwnd)); //!< rectを更新する
			}
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT Window::WindowProcSub(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	//!< windowのインスタンスを取得
	RefPtr<Window> window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	Message message = Window::GetMessageCode(msg); //!< WindowsのメッセージコードをWindow::Messageに変換する

	if (window != nullptr) {
		//!< Resizeは消費されるまで保持する. 毎messageでNoneに戻すと, resizeの直後に来る他のmessageで
		//!< 消えてしまい, swap chainがwindowのサイズに追従できない. (Window::ConsumeEventで消費する)
		if (window->event_ != Event::Resize) {
			window->event_ = Event::None; //!< eventを初期化する
		}
	}

	//!< borderless windowの非クライアント領域処理 (borderlessでない場合は何も行わない)
	if (window != nullptr) {
		LRESULT result = 0;
		if (window->ProcessBorderlessMessage(hwnd, message, wparam, lparam, result)) {
			return result;
		}
	}

	switch (message) {
		case Message::Create:
			{ //!< windowが生成された場合, instanceをウィンドウに関連付ける
				CREATESTRUCT* structure = reinterpret_cast<CREATESTRUCT*>(lparam);
				LONG_PTR instance       = reinterpret_cast<LONG_PTR>(structure->lpCreateParams);
				SetWindowLongPtr(hwnd, GWLP_USERDATA, instance);
			}
			break;

		case Message::Destroy:
			{ //!< windowを破棄する
				DestroyWindow(hwnd);
				return 0;
			}

		case Message::EnterSizeMove:
			{ //!< サイズ変更や移動のmodal loopに入った場合, loop中のWM_SIZEを無視するようにする
				if (window != nullptr) {
					window->isSizeMoving_ = true;
				}
			}
			break;

		case Message::ExitSizeMove:
			{ //!< windowのサイズ変更や移動が終了した場合, instanceにリサイズ処理を行う
				if (window != nullptr) {
					window->isSizeMoving_ = false;
					window->UpdateRect(Window::GetCurrentClientRect(hwnd)); //!< rectを更新する
				}
			}
			break;

		case Message::Size:
			{ //!< 最大化 / スナップ / SetWindowPosはmodal loopを通らずWM_EXITSIZEMOVEが来ないため,
			  //!< ここでrectを更新する. これが無いとGetClient()が古い値を返し, swap chainが追従しない.
				if (window == nullptr || window->isSizeMoving_) {
					break; //!< drag中はloopの終了時にまとめて処理する
				}

				if (wparam == SIZE_MINIMIZED) {
					break; //!< 最小化ではclient領域が0になるためresizeしない
				}

				window->UpdateRect(Window::GetCurrentClientRect(hwnd)); //!< rectを更新する
			}
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

WNDPROC Window::GetWindowProcFunction(Category category) {
	switch (category) {
		case Sxx::Platform::Window::Category::Main:
			return Window::WindowProcMain;

		case Sxx::Platform::Window::Category::Sub:
			return Window::WindowProcSub;

		default:
			StreamLogger::Exception("category is not valid.");
	}
}

bool Window::ProcessBorderlessMessage(HWND hwnd, Message message, WPARAM wparam, LPARAM lparam, LRESULT& result) {
	if (!isBorderless_) {
		return false; //!< borderlessでない場合は既存の挙動を一切変更しない
	}

	switch (message) {
		case Message::NcCalcSize:
			{ //!< 非クライアント領域を消し, クライアント領域をwindow全体に広げる
				if (wparam != TRUE) {
					break; //!< rgrc[0]のみが有効な形式でない場合は既定の処理に任せる
				}

				NCCALCSIZE_PARAMS* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lparam);

				if (IsZoomed(hwnd)) {
					//!< 最大化時のwindow矩形はframeの分だけwork areaより大きい.
					//!< そのままクライアントにするとtaskbarを覆ってしまうため, frame分だけ内側に寄せる.
					const LONG frameX = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
					const LONG frameY = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);

					params->rgrc[0].left   += frameX;
					params->rgrc[0].right  -= frameX;
					params->rgrc[0].top    += frameY;
					params->rgrc[0].bottom -= frameY;
				}

				//!< rgrc[0]を書き換えずに返すことで, 非クライアント領域が無くなる
				result = 0;
				return true;
			}

		case Message::NcHitTest:
			{ //!< WS_CAPTIONが無いためリサイズ枠とcaptionを自前で返す
				result = Window::HitTestNonClient(hwnd, lparam, nonClientHitTest_);
				return true;
			}

		case Message::SetCursor:
			{ //!< クライアント領域のカーソル形状をアプリ側の要求で決める
				if (LOWORD(lparam) != HTCLIENT || cursorQuery_ == nullptr) {
					break; //!< リサイズ枠などはOSの矢印カーソルに任せる
				}

				POINT point = {};
				if (!GetCursorPos(&point) || !ScreenToClient(hwnd, &point)) {
					break;
				}

				const CursorShape shape = cursorQuery_(
					Vector2i{ static_cast<std::int32_t>(point.x), static_cast<std::int32_t>(point.y) }
				);

				SetCursor(LoadCursorW(nullptr, Window::ConvertCursorResource(shape)));

				result = TRUE;
				return true;
			}

		default:
			break;
	}

	return false;
}

LRESULT Window::HitTestNonClient(HWND hwnd, LPARAM lparam, const NonClientHitTest& function) {
	//!< lparamはscreen座標なのでclient座標へ変換する
	POINT point = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	ScreenToClient(hwnd, &point);

	RECT client = Window::GetCurrentClientRect(hwnd);

	if (!IsZoomed(hwnd)) {
		//!< 最大化中はリサイズできないため枠判定を行わない
		const bool left   = point.x <  client.left   + kResizeBorder;
		const bool right  = point.x >= client.right  - kResizeBorder;
		const bool top    = point.y <  client.top    + kResizeBorder;
		const bool bottom = point.y >= client.bottom - kResizeBorder;

		//!< 角を先に判定する. (辺を先に返すと角が掴めない)
		if (top    && left)  { return HTTOPLEFT; }
		if (top    && right) { return HTTOPRIGHT; }
		if (bottom && left)  { return HTBOTTOMLEFT; }
		if (bottom && right) { return HTBOTTOMRIGHT; }

		//!< 枠判定はcaption判定より先に行う
		if (left)   { return HTLEFT; }
		if (right)  { return HTRIGHT; }
		if (top)    { return HTTOP; }
		if (bottom) { return HTBOTTOM; }
	}

	if (function == nullptr) {
		return HTCLIENT; //!< callback未設定の場合はすべてクライアント領域として扱う
	}

	switch (function(Vector2i{ static_cast<std::int32_t>(point.x), static_cast<std::int32_t>(point.y) })) {
		case NonClientArea::Caption:
			return HTCAPTION; //!< dragでの移動, double clickでの最大化, snapをOSに任せる

		case NonClientArea::Client:
		default:
			return HTCLIENT;
	}
}

LPCWSTR Window::ConvertCursorResource(CursorShape shape) {
	switch (shape) {
		case CursorShape::SizeWE: return IDC_SIZEWE;
		case CursorShape::SizeNS: return IDC_SIZENS;
		case CursorShape::Hand:   return IDC_HAND;
		case CursorShape::IBeam:  return IDC_IBEAM;

		case CursorShape::Arrow:
		default:                  return IDC_ARROW;
	}
}

RECT Window::ConvertClientRect(const Vector2ui& client) {
	RECT rect = {};
	rect.right  = static_cast<LONG>(client.x);
	rect.bottom = static_cast<LONG>(client.y);

	return rect;
}

Vector2ui Window::ConvertClientSize(const RECT& rect) {
	Vector2ui client = {};
	client.x = static_cast<std::uint32_t>(rect.right - rect.left);
	client.y = static_cast<std::uint32_t>(rect.bottom - rect.top);

	return client;
}

RECT Window::GetCurrentClientRect(HWND hwnd) {
	RECT rect = {};
	GetClientRect(hwnd, &rect);

	return rect;
}

void Window::UpdateRect(const RECT& rect) {
	Vector2ui previous = Window::ConvertClientSize(rect_);
	Vector2ui current  = Window::ConvertClientSize(rect);

	if (Comparison::All(previous == current)) {
		return; //!< サイズが変更されていない場合はリサイズ処理を行わない
	}

	rect_ = rect; //!< rectを更新する

	event_ = Event::Resize; //!< リサイズイベントを発生させる

	StreamLogger::Debug(
		L"Platform::Window | window resized. name: {}, hwnd: {:p}, client: {}",
		name_, static_cast<const void*>(hwnd_), Window::ConvertClientSize(rect_)
	);
}

Window::Message Window::GetMessageCode(UINT msg) {
	return static_cast<Message>(msg); //!< WindowsのメッセージコードをWindow::Messageに変換する
}

std::wstring Window::GetWindowClassName(const std::wstring& name) {
	return L"[Sxavenger Engine Platform::Window] " + name;
}
