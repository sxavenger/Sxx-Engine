#include "Window.h"
SXAVENGER_ENGINE_USING_(Platform)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Math/VectorComparison.h>
#include <Lib/Pointer/ReferencePointer.h>

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

	//!< ウィンドウサイズの調整
	AdjustWindowRect(&rect, static_cast<DWORD>(style_), false);
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
		window->event_ = Event::None; //!< eventを初期化する
	}

	//if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
	//	return true;
	//}

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

		case Message::ExitSizeMove:
			{ //!< windowのサイズ変更や移動が終了した場合, instanceにリサイズ処理を行う
				if (wparam == SIZE_MINIMIZED) {
					break; //!< 最小化されたときはリサイズ処理を行わない
				}

				if (window != nullptr) {
					window->UpdateRect(Window::GetCurrentClientRect(hwnd)); //!< rectを更新する
				}
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
		window->event_ = Event::None; //!< eventを初期化する
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

		case Message::ExitSizeMove:
			{ //!< windowのサイズ変更や移動が終了した場合, instanceにリサイズ処理を行う
				if (wparam == SIZE_MINIMIZED) {
					break; //!< 最小化されたときはリサイズ処理を行わない
				}
				
				if (window != nullptr) {
					window->UpdateRect(Window::GetCurrentClientRect(hwnd)); //!< rectを更新する
				}
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
