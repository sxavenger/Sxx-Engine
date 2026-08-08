#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Flag/Flag.h>
#include <Lib/Math/Vector2.h>

//* windows
#include <windows.h>

//* c++
#include <cstdint>
#include <string>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Platform)

////////////////////////////////////////////////////////////////////////////////////////////
// Window class
////////////////////////////////////////////////////////////////////////////////////////////
class Window final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Style enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Style : DWORD {
		Overlapped  = WS_OVERLAPPED,  //!< 通常windowスタイル.
		Popup       = WS_POPUP,       //!< フルスクリーンスタイル.
		Child       = WS_CHILD,       //!< 子windowスタイル.
		Titlebar    = WS_CAPTION,     //!< タイトルバーを表示するスタイル.
		SystemMenu  = WS_SYSMENU,     //!< システムメニューを表示するスタイル.
		Resizeable  = WS_THICKFRAME,  //!< サイズ変更可能なスタイル.
		MinimizeBox = WS_MINIMIZEBOX, //!< 最小化ボタンを表示するスタイル.
		MaximizeBox = WS_MAXIMIZEBOX, //!< 最大化ボタンを表示するスタイル.

		Default = Overlapped | Titlebar | SystemMenu | Resizeable | MinimizeBox | MaximizeBox //!< デフォルトのスタイル.
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mode enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Mode : uint8_t {
		Window //!< ウィンドウモード
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Category enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Category : bool {
		Main, //!< メインウィンドウ
		Sub,  //!< サブウィンドウ
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Message enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Message : UINT {
		Null          = WM_NULL,          //!< メッセージなし [WM_NULL](https://learn.microsoft.com/ja-jp/windows/win32/winmsg/wm-null)
		Create        = WM_CREATE,        //!< windowが生成された場合 [WM_CREATE](https://learn.microsoft.com/ja-jp/windows/win32/winmsg/wm-create)
		Destroy       = WM_DESTROY,       //!< windowが破棄された場合 [WM_DESTROY](https://learn.microsoft.com/ja-jp/windows/win32/winmsg/wm-destroy)
		Size          = WM_SIZE,          //!< windowのサイズが変更された場合 [WM_SIZE](https://learn.microsoft.com/ja-jp/windows/win32/winmsg/wm-size)
		EnterSizeMove = WM_ENTERSIZEMOVE, //!< windowのサイズ変更や移動が開始された場合 [WM_ENTERSIZEMOVE](https://learn.microsoft.com/ja-jp/windows/win32/winmsg/wm-entersizemove)
		ExitSizeMove  = WM_EXITSIZEMOVE,  //!< windowのサイズ変更や移動が終了した場合 [WM_EXITSIZEMOVE](https://learn.microsoft.com/ja-jp/windows/win32/winmsg/wm-exitsizemove)
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Event enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Event : uint8_t {
		None,
		Resize, //!< ウィンドウサイズが変更された
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	Window() noexcept = default;
	~Window() { Close(); }

	//* window option *//

	void Create(
		const std::wstring& name, const Vector2ui& client, Category category,
		const FlagEnum<Style>& style = Style::Default
	);

	void Reset();

	void Show(DWORD flag = SW_SHOW) const;

	void Open() const;

	void Close();

	//! @brief ウィンドウアイコンを設定する
	void SetWindowIcon(const std::filesystem::path& filepath, const Vector2ui& size) const;

	//! @brief タスクバーアイコンを設定する
	void SetTaskbarIcon(const std::filesystem::path& filepath, const Vector2ui& size) const;

	//! @brief ウィンドウとタスクバーのアイコンを設定する
	void SetIcon(const std::filesystem::path& filepath, const Vector2ui& size) const;

	bool IsOpen() const;

	//* window state option *//

	HWND GetHwnd() const { return hwnd_; }

	Vector2ui GetClient() const { return Window::ConvertClientSize(rect_); }

	Event GetEvent() const { return event_; }

	//* operator [copy] <Window> (delete) *//

	Window(const Window&)            = delete;
	Window& operator=(const Window&) = delete;

	//* operator [move] <Window> *//

	Window(Window&&) noexcept            = default;
	Window& operator=(Window&&) noexcept = default;

	//* window static method *//

	//! @brief メッセージ処理
	//! @retval true  メッセージが存在する
	//! @retval false mainのwindowが閉じられた
	static bool ProcessMessage();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* window state *//

	HINSTANCE hinst_;
	HWND      hwnd_;

	RECT rect_ = {};

	//* window parameter *//

	Mode mode_;
	Category category_;

	std::wstring name_;

	FlagEnum<Style> style_;

	//* runtime state *//

	Event event_ = Event::None;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* window procedure helper methods *//

	static LRESULT CALLBACK WindowProcMain(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static LRESULT CALLBACK WindowProcSub(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static WNDPROC GetWindowProcFunction(Category category);

	//* window style helper methods *//

	static RECT ConvertClientRect(const Vector2ui& client);

	static Vector2ui ConvertClientSize(const RECT& rect);

	static RECT GetCurrentClientRect(HWND hwnd);

	void UpdateRect(const RECT& rect);

	//* window message helper methods *//

	static Message GetMessageCode(UINT msg);

	//* window name helper methods *//

	static std::wstring GetWindowClassName(const std::wstring& name);

};

SXAVENGER_ENGINE_NAMESPACE_END
