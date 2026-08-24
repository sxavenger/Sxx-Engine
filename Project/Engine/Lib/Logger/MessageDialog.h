#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "LoggerUtil.h"

//* lib
#include <Lib/CXXAttribute.hpp>

//* windows
#include <windows.h>
#include <WinUser.h>

//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// MessageDialog class
////////////////////////////////////////////////////////////////////////////////////////////
class MessageDialog final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Icon enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief MessageBoxのアイコン.
	enum class Icon : UINT {
		Exclamation = MB_ICONEXCLAMATION,
		Warning     = MB_ICONWARNING,
		Information = MB_ICONINFORMATION,
		Asterisk    = MB_ICONASTERISK,
		Question    = MB_ICONQUESTION,
		Stop        = MB_ICONSTOP,
		Error       = MB_ICONERROR,
		Hand        = MB_ICONHAND,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Button enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief MessageBoxのボタン.
	enum class Button : UINT {
		AbortRetryIgnore  = MB_ABORTRETRYIGNORE,
		CancelTryContinue = MB_CANCELTRYCONTINUE,
		Help              = MB_HELP,
		Ok                = MB_OK,
		OkCancel          = MB_OKCANCEL,
		RetryCancel       = MB_RETRYCANCEL,
		YesNo             = MB_YESNO,
		YesNoCancel       = MB_YESNOCANCEL,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief MessageBoxAを表示する.
	static void ShowA(const std::string_view& caption, const std::string_view& text, Icon icon = Icon::Information, Button button = Button::Ok);

	//! @brief MessageBoxWを表示する.
	static void ShowW(const std::wstring_view& caption, const std::wstring_view& text, Icon icon = Icon::Information, Button button = Button::Ok);

private:
};
