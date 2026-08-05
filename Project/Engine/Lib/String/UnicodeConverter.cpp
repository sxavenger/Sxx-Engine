#include "UnicodeConverter.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <comdef.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UnicodeConverter class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string UnicodeConverter::ConvertA(const std::wstring_view& wstr) {
	if (wstr.empty()) {
		return {};
	}

	int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);

	if (size == 0) {
		return std::string{};
	}

	//!< stringへの変換
	std::string string(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), string.data(), size, NULL, NULL);

	return string;
}

std::wstring UnicodeConverter::ConvertW(const std::string_view& str) {
	if (str.empty()) {
		return std::wstring{};
	}

	int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0);

	if (size == 0) {
		return std::wstring{};
	}

	//!< wstringへの変換
	std::wstring wstring(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), wstring.data(), size);

	return wstring;
}
