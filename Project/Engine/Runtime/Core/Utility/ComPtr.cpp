#include "ComPtr.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <comdef.h>

//* lib
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// ComPtrUtil namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

std::wstring_view ComPtrUtil::GetComErrorMessage(HRESULT hr) {
	if (SUCCEEDED(hr)) {
		return std::wstring_view();
	}

	return _com_error(hr).ErrorMessage();
}

void ComPtrUtil::Assert(HRESULT hr, const std::wstring_view& message) {
	StreamLogger::Assert(SUCCEEDED(hr), message, std::format(L"_com_error: {}", GetComErrorMessage(hr)));
}
