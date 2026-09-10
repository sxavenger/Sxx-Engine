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

std::wstring ComPtrUtil::GetComErrorMessage(HRESULT hr) {
	if (SUCCEEDED(hr)) {
		return std::wstring();
	}

	return _com_error(hr).ErrorMessage();
}

void ComPtrUtil::Assert(HRESULT hr, const std::wstring_view& message) {
	STREAM_ASSERT_SUMMARY(SUCCEEDED(hr), message, L"_com_error: {}", GetComErrorMessage(hr));
}
