#include "WinApp.h"
SXAVENGER_ENGINE_USING_(Platform)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Core/Utility/ComPtr.h>

//* lib
#include <Lib/Logger/StreamLogger.h>

//* windows
#include <shobjidl.h>
#include <shellapi.h>
#include <shlobj.h>

////////////////////////////////////////////////////////////////////////////////////////////
// WinApp class methods
////////////////////////////////////////////////////////////////////////////////////////////

void WinApp::Init() {
	CoInitializeEx(0, COINIT_MULTITHREADED);
	timeBeginPeriod(1);

	//MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);

	STREAM_LOG_INFO("Platform::WinApp | win app initialized.");
}

void WinApp::Term() {
	//MFShutdown();
	
	CoUninitialize();

	STREAM_LOG_INFO("Platform::WinApp | win app terminated.");
}

std::optional<std::filesystem::path> WinApp::GetSaveFilepath(const std::wstring& title, const std::filesystem::path& current, const std::pair<std::wstring, std::wstring>& filter, const std::filesystem::path& extension) {

	ComPtr<IFileSaveDialog> dialog;
	auto hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dialog));

	dialog->SetTitle(title.c_str());

	ComPtr<IShellItem> folder;
	SHCreateItemFromParsingName(current.wstring().c_str(), nullptr, IID_PPV_ARGS(&folder));
	dialog->SetFolder(folder.Get());

	COMDLG_FILTERSPEC type = { filter.first.c_str(), filter.second.c_str() };
	dialog->SetFileTypes(1, &type);
	dialog->SetDefaultExtension(extension.wstring().c_str());

	hr = dialog->Show(nullptr);
	if (FAILED(hr)) {
		return std::nullopt;
	}

	ComPtr<IShellItem> result;
	dialog->GetResult(&result);

	LPWSTR path = nullptr;
	hr = result->GetDisplayName(SIGDN_FILESYSPATH, &path);

	if (SUCCEEDED(hr)) {
		std::filesystem::path filepath(path);
		CoTaskMemFree(path);
		return filepath;
	}

	return {};

}

std::optional<std::filesystem::path> WinApp::GetOpenFilepath(const std::wstring& title, const std::filesystem::path& current, const std::pair<std::wstring, std::wstring>& filter) {

	ComPtr<IFileOpenDialog> dialog;
	auto hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dialog));

	dialog->SetTitle(title.c_str());

	ComPtr<IShellItem> folder;
	SHCreateItemFromParsingName(current.wstring().c_str(), nullptr, IID_PPV_ARGS(&folder));
	dialog->SetFolder(folder.Get());

	const COMDLG_FILTERSPEC type = { filter.first.c_str(), filter.second.c_str() };
	dialog->SetFileTypes(1, &type);

	hr = dialog->Show(nullptr);

	if (FAILED(hr)) {
		return std::nullopt;
	}

	ComPtr<IShellItem> result;
	hr = dialog->GetResult(&result);

	LPWSTR path = nullptr;
	hr = result->GetDisplayName(SIGDN_FILESYSPATH, &path);

	if (SUCCEEDED(hr)) {
		std::filesystem::path filepath(path);
		CoTaskMemFree(path);
		return filepath;
	}

	return {};
}

bool WinApp::OpenExplorer(const std::filesystem::path& filepath) {
	// FIXME: Exprorer起動までの時間が長い.

	PIDLIST_ABSOLUTE pidl = nullptr;

	HRESULT hr = SHParseDisplayName(
		std::filesystem::absolute(filepath).c_str(),
		nullptr,
		&pidl,
		0,
		nullptr
	);

	if (FAILED(hr) || pidl == nullptr) {
		return false;
	}

	hr = SHOpenFolderAndSelectItems(
		pidl,
		0,
		nullptr,
		0
	);

	CoTaskMemFree(pidl);
	return SUCCEEDED(hr);

}

bool WinApp::OpenApplication(const std::filesystem::path& filepath) {

	HINSTANCE result = ShellExecuteW(
		nullptr,
		L"open",
		filepath.c_str(),
		nullptr,
		nullptr,
		SW_SHOWNORMAL
	);

	return reinterpret_cast<intptr_t>(result) > 32;
}

bool WinApp::SetClipboardText(const std::wstring_view& text) {

	if (!OpenClipboard(nullptr)) {
		return false; //!< 他のアプリケーションがクリップボードを使用中の場合、OpenClipboardは失敗する.
	}

	EmptyClipboard(); //!< 現在のクリップボードの内容を削除する.

	//!< クリップボードにコピーする文字列のサイズを計算する. (終端文字を含む)
	const size_t size = (text.size() + 1) * sizeof(wchar_t);

	//!< クリップボードにコピーする文字列のためのメモリを確保する.
	HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, size);
	if (memory == nullptr) {
		CloseClipboard();
		return false;
	}

	{
		//!< 確保したメモリをロックして書き込み可能なアドレスを取得する
		void* data = GlobalLock(memory);
		if (data == nullptr) {
			GlobalFree(memory);
			CloseClipboard();
			return false;
		}

		std::memcpy(data, text.data(), text.size() * sizeof(wchar_t));
		static_cast<wchar_t*>(data)[text.size()] = L'\0'; //!< 終端文字を追加する.

		GlobalUnlock(memory);
	}

	//!< Unicode文字列をクリップボードに設定する.
	if (SetClipboardData(CF_UNICODETEXT, memory) == nullptr) {
		//!< 登録に失敗した場合は、呼び出し側でメモリを解放する
		GlobalFree(memory);
		CloseClipboard();
		return false;
	}

	CloseClipboard();

	// note: SetClipboardData() 成功後は memory の所有権がシステムへ移るため、ここでは GlobalFree() しない.
	return true;
}

std::optional<std::wstring> WinApp::GetClipboardText() {

	if (!OpenClipboard(nullptr)) {
		return std::nullopt; //!< 他のアプリケーションがクリップボードを使用中の場合、OpenClipboardは失敗する.
	}

	//!< Unicodeテキスト形式のデータを取得する
	HANDLE handle = GetClipboardData(CF_UNICODETEXT);
	if (handle == nullptr) {
		CloseClipboard();
		return std::nullopt;
	}

	std::wstring text = L"";

	{
		const wchar_t* data = static_cast<const wchar_t*>(GlobalLock(handle));
		if (data == nullptr) {
			CloseClipboard();
			return std::nullopt;
		}

		text = data;

		GlobalUnlock(handle);
	}

	CloseClipboard();
	return text;
}
