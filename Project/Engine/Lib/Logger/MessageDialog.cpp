#include "MessageDialog.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MessageDialog class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MessageDialog::ShowA(const std::string_view& caption, const std::string_view& text, Icon icon, Button button) {
	MessageBoxA(
		NULL,
		text.data(),
		caption.data(),
		static_cast<UINT>(button) | static_cast<UINT>(icon) | MB_TASKMODAL
	);
}

void MessageDialog::ShowW(const std::wstring_view& caption, const std::wstring_view& text, Icon icon, Button button) {
	MessageBoxW(
		NULL,
		text.data(),
		caption.data(),
		static_cast<UINT>(button) | static_cast<UINT>(icon) | MB_TASKMODAL
	);
}
