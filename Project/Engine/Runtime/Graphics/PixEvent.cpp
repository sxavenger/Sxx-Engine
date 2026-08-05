#include "PixEvent.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Toml/TomlReader.h>

//* pix
#ifdef DEVELOPMENT
#define USE_PIX
#include <PixEvents/pix3.h>
#else
#include <pix.h>
#endif

//* c++
#include <stack>
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// [PixEvent] Settings structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PixEvent::Settings::Parse(const toml::table& config) {
	TomlReader<bool>::Find(config, "enable", enable); //!< pixを有効にするかの取得.
}

PixEvent::Settings PixEvent::Settings::ParseFromConfig(const Configuration& config) {

	Settings settings;

	if (!config.Contains(kConfigPath.GetPath())) {
		StreamLogger::Warning(
			"Graphics::PixEvent::Settings | config does not exist. path: {}", kConfigPath.GetPath()
		);
		return settings; //!< 設定が存在しない.
	}

	settings.Parse(config.GetConfig(Settings::kConfigPath.GetPath()));        //!< 全体設定の取得.
	settings.Parse(config.GetConfig(Settings::kConfigPath.GetProfilePath())); //!< プロファイル設定の取得.

	return settings;
}

void PixEvent::Settings::Log(const Settings& settings) {
	StreamLogger::Debug("Graphics::PixEvent::Settings | enable: {}", settings.enable);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PixEvent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PixEvent::Init(const Configuration& config) {

	Settings settings = Settings::ParseFromConfig(config); //!< 設定の取得.
	Settings::Log(settings);                               //!< 設定のログ出力.

	if (!settings.enable) {
		return; //!< pixが無効の場合は何もしない.
	}

	directory_ = PixEvent::FindPixDirectory(); //!< pixのlibファイルdDirectoryを探索する.

	PixEvent::LoadPixModule(directory_, L"WinPixGpuCapturer.dll"); //!< pixのmoduleを読み込む. (DirectX12の初期化前に行う)

	PixEvent::PIXSetHUDOptionsImpl(HUDOptions::ShowOnNoWindows); //!< [pix3.h] PIXのHUDを全てのwindowで非表示にする.
}

void PixEvent::BeginEvent(ID3D12GraphicsCommandList* commandList, const std::wstring_view& name, uint8_t indent) {
#ifdef USE_PIX
	PIXBeginEvent(commandList, PIX_COLOR_INDEX(indent), name.data()); //!< [pix3.h]
#else
	PIXBeginEvent(commandList, NULL, name.data()); //!< [pix.h]
#endif
}

void PixEvent::EndEvent(ID3D12GraphicsCommandList* commandList) {
#ifdef USE_PIX
	PIXEndEvent(commandList); //!< [pix3.h]
#else
	PIXEndEvent(commandList); //!< [pix.h]
#endif
}

void PixEvent::CaptureNextFrames(const std::filesystem::path& filepath, uint32_t frames) {
	if (filepath.extension() != L".wpix") {
		StreamLogger::Error("Graphics::PixEvent | capture failed. invalid file extension. filepath: {}", filepath.generic_string());
		return; //!< 拡張子が.wpixでない場合は何もしない
	}

#ifdef USE_PIX
	//!< [pix3.h] 自作関数PIXGpuCaptureNextFramesImplを呼び出す. (WinPixGpuCapturer.dllのCaptureNextFrame関数を呼び出す)

	auto hr = PixEvent::PIXGpuCaptureNextFramesImpl(filepath, frames);
	if (FAILED(hr)) {
		StreamLogger::Error(L"Graphics::PixEvent | capture frame failed. filepath: {}, _com_error: {}", filepath.generic_wstring(), ComPtrUtil::GetComErrorMessage(hr));
		return;
	}

	StreamLogger::Info("Graphics::PixEvent | pix captured. filepath: {}", filepath.generic_string());
#else
	//!< [pix.h] pix.hにはGPUキャプチャのAPIがないため、何もしない
#endif
}

std::filesystem::path PixEvent::FindPixDirectory() {
#ifdef USE_PIX
	static const std::filesystem::path kPixDirectory   = L"C:/Program Files/Microsoft PIX"; //!< pixがinstallされるdirectory
	static const std::filesystem::path kPixApplication = L"WinPix.exe";

	//!< kPixDirectory / <version> / kPixApplication の有効versionを探す.

	if (!std::filesystem::exists(kPixDirectory)) {
		StreamLogger::Warning(std::format("Graphics::PixEvent | pix version directory not found. directory: {}", kPixDirectory.generic_string()));
		return {};
	}

	std::stack<std::filesystem::path> version;
	for (const auto& entry : std::filesystem::directory_iterator(kPixDirectory) | std::views::filter([](const auto& entry) { return entry.is_directory(); })) { //!< directoryのみを対象.
		//!< 古いversionから順に探索するため、stackにpushする. (stackのtopが最新versionになる)
		version.emplace(entry.path());
	}

	while (!version.empty()) {
		std::filesystem::path current = version.top();
		version.pop();

		if (std::filesystem::exists(current / kPixApplication)) {
			StreamLogger::Info(std::format("Graphics::PixEvent | pix enable version directory found. directory: {}", current.generic_string()));
			return current; //!< WinPix.exeが存在するversionが見つかった時点で探索を終了する.
		}
	}

	StreamLogger::Warning(std::format("Graphics::PixEvent | pix enable version directory not found."));
	return {};
#endif
}

void PixEvent::LoadPixModule(const std::filesystem::path& directory, const std::filesystem::path& filename) {
#ifdef USE_PIX
	if (directory.empty()) {
		return; //!< directoryが見つかっていない場合は何もしない.
	}

	const std::filesystem::path filepath = directory / filename;

	HMODULE module = GetModuleHandleW(filepath.generic_wstring().c_str());
	if (module != NULL) {
		return; //!< 既に読み込まれている場合は何もしない.
	}

	LoadLibraryW(filepath.generic_wstring().c_str()); //!< moduleを読み込む.
	
#endif
}

void* PixEvent::GetGpuCaptureFunctionPointer(const std::string& function) {
#ifdef USE_PIX
	//!< [pix3.h] PixImpl::GetGpuCaptureFunctionPtr関数を参照.

	if (directory_.empty()) {
		return nullptr; //!< directoryが見つかっていない場合はnullptrを返す.
	}

	const std::filesystem::path filepath = directory_ / L"WinPixGpuCapturer.dll";
	//!< 事前にdirectoryに格納されたversionのWinPixGpuCapturer.dllを対象とする.

	HMODULE module = GetModuleHandleW(filepath.generic_wstring().c_str());
	if (module == NULL) {
		return nullptr; //!< moduleが読み込まれていない場合はnullptrを返す.
	}

	auto fn = (void*)GetProcAddress(module, function.c_str());
	if (fn == nullptr) {
		return nullptr;
	}

	return fn;

#else
	return nullptr; //!< [pix.h] pix.hにはGPUキャプチャのAPIがないため、常にnullptrを返す
#endif
}

HRESULT WINAPI PixEvent::PIXSetHUDOptionsImpl(HUDOptions options) {
#ifdef USE_PIX
	//!< [pix3.h] PIXSetHUDOptions関数を参照.
	
	typedef HRESULT(WINAPI* SetHUDOptionsFn)(PIXHUDOptions);

	auto fn = (SetHUDOptionsFn)PixEvent::GetGpuCaptureFunctionPointer("SetHUDOptions");
	if (fn == nullptr) {
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return fn(static_cast<PIXHUDOptions>(options));

#else
	return E_NOTIMPL; //!< [pix.h] pix.hにはHUDオプションのAPIがないため、常にE_NOTIMPLを返す
#endif
}

HRESULT WINAPI PixEvent::PIXGpuCaptureNextFramesImpl(const std::filesystem::path& filepath, uint32_t frames) {
#ifdef USE_PIX
	//!< [pix3.h] PIXGpuCaptureNextFrames関数を参照.

	typedef HRESULT(WINAPI* CaptureNextFrameFn)(PCWSTR, UINT32);

	auto fn = (CaptureNextFrameFn)PixEvent::GetGpuCaptureFunctionPointer("CaptureNextFrame");
	if (fn == nullptr) {
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return fn(filepath.generic_wstring().c_str(), frames);
	
#else
	return E_NOTIMPL; //!< [pix.h] pix.hにはGPUキャプチャのAPIがないため、常にE_NOTIMPLを返す
#endif
}
