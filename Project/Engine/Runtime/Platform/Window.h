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
#include <functional>

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

		Borderless = Overlapped | SystemMenu | Resizeable | MinimizeBox | MaximizeBox, //!< OSのタイトルバーを出さないスタイル. WM_NCCALCSIZEで非クライアント領域を潰す.

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

		//* borderless window用 *//

		NcCalcSize    = WM_NCCALCSIZE,    //!< 非クライアント領域のサイズが計算される場合 [WM_NCCALCSIZE](https://learn.microsoft.com/ja-jp/windows/win32/winmsg/wm-nccalcsize)
		NcHitTest     = WM_NCHITTEST,     //!< カーソル位置がwindowのどの領域か問われた場合 [WM_NCHITTEST](https://learn.microsoft.com/ja-jp/windows/win32/inputdev/wm-nchittest)
		SetCursor     = WM_SETCURSOR,     //!< カーソル形状の設定を問われた場合 [WM_SETCURSOR](https://learn.microsoft.com/ja-jp/windows/win32/menurc/wm-setcursor)
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Event enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Event : uint8_t {
		None,
		Resize, //!< ウィンドウサイズが変更された
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// NonClientArea enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief borderless windowで, 座標がどの領域に当たるかをアプリ側が返すための種別.
	enum class NonClientArea : uint8_t {
		Client,  //!< 通常のクライアント領域. (widgetが入力を受ける)
		Caption, //!< タイトルバー相当. dragでwindowが移動し, double clickで最大化する.
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// CursorShape enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief borderless windowのクライアント領域で表示するカーソル形状.
	//! @note Platform層はEditor::Slateに依存できないため, Editor::Slate::Cursorと1:1で対応する独自の種別を持つ.
	enum class CursorShape : uint8_t {
		Arrow,  //!< Slate::Cursor::Default
		SizeWE, //!< Slate::Cursor::ResizeLeftRight
		SizeNS, //!< Slate::Cursor::ResizeUpDown
		Hand,   //!< Slate::Cursor::Hand
		IBeam,  //!< Slate::Cursor::TextEdit
	};

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	//! @brief borderless windowのヒットテストをアプリ側へ委譲する.
	//! @param client クライアント座標系の点.
	using NonClientHitTest = std::function<NonClientArea(const Vector2i& client)>;

	//! @brief borderless windowのカーソル形状をアプリ側へ問い合わせる.
	//! @param client クライアント座標系の点.
	using CursorQuery = std::function<CursorShape(const Vector2i& client)>;

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

	//! @brief eventを消費して None に戻す.
	//! @note eventは消費されるまで保持される. ProcessMessage()は溜まったmessageを一括で処理するため,
	//!       毎messageでNoneに戻すとresize直後の他のmessage(borderlessではmouse移動ごとに飛ぶ
	//!       WM_NCHITTEST)でResizeが消え, swap chainが追従できなくなる.
	void ConsumeEvent() { event_ = Event::None; }

	//* borderless window option *//

	//! @brief OSのタイトルバーを持たないwindowか
	//! @note Create()に渡されたstyleから決定する. falseの場合, 非クライアント領域の処理は一切行わない.
	bool IsBorderless() const { return isBorderless_; }

	//! @brief borderless windowのヒットテストをアプリ側へ委譲する
	//! @note 未設定の場合, リサイズ枠以外はすべてクライアント領域として扱う.
	void SetNonClientHitTest(const NonClientHitTest& function) { nonClientHitTest_ = function; }

	//! @brief borderless windowのカーソル形状をアプリ側へ問い合わせる
	//! @note 未設定の場合, OSの既定のカーソル処理に任せる.
	void SetCursorQuery(const CursorQuery& function) { cursorQuery_ = function; }

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

	bool isBorderless_ = false; //!< OSのタイトルバーを持たないwindowか

	//* borderless window state *//

	NonClientHitTest nonClientHitTest_ = nullptr;
	CursorQuery      cursorQuery_      = nullptr;

	//* runtime state *//

	Event event_ = Event::None;

	//!< size/moveのmodal loop中かどうか. (WM_ENTERSIZEMOVE 〜 WM_EXITSIZEMOVE)
	//!< loop中のWM_SIZEは無視し, loopの終了時に一度だけresizeする. (drag中に毎frame再生成しないため)
	bool isSizeMoving_ = false;

	//* borderless window constant *//

	static constexpr LONG kResizeBorder = 8; //!< リサイズ枠として扱うクライアント端からの太さ[px].

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* window procedure helper methods *//

	static LRESULT CALLBACK WindowProcMain(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static LRESULT CALLBACK WindowProcSub(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static WNDPROC GetWindowProcFunction(Category category);

	//* borderless window helper methods *//

	//! @brief borderless windowの非クライアント領域messageを処理する
	//! @param result 処理した場合に返すべき戻り値
	//! @retval true  messageを処理した. resultをそのまま返す
	//! @retval false 未処理. 既定の処理へ進む
	//! @note borderlessでない場合は何も処理せず必ずfalseを返す.
	bool ProcessBorderlessMessage(HWND hwnd, Message message, WPARAM wparam, LPARAM lparam, LRESULT& result);

	//! @brief borderless windowのヒットテストを行う
	//! @param lparam WM_NCHITTESTのlparam. (screen座標)
	static LRESULT HitTestNonClient(HWND hwnd, LPARAM lparam, const NonClientHitTest& function);

	//! @brief CursorShapeをwindowsのカーソルリソースへ変換する
	static LPCWSTR ConvertCursorResource(CursorShape shape);

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
