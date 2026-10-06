#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* windows
#include <wrl.h>
#include <comdef.h>

//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
template <typename T> requires std::derived_from<T, IUnknown>
using ComPtr = Microsoft::WRL::ComPtr<T>;

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

static std::wstring GetComErrorMessage(HRESULT hr) {
	if (SUCCEEDED(hr)) {
		return std::wstring();
	}

	return _com_error(hr).ErrorMessage();
}

SXAVENGER_ENGINE_NAMESPACE_END

