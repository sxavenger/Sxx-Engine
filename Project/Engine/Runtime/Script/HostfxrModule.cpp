#include "HostfxrModule.h"
SXAVENGER_ENGINE_USING_(Script)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Toml/TomlReader.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [HostfxrModule] Setting structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void HostfxrModule::Settings::Parse(const toml::table& config) {
	TomlReader<std::filesystem::path>::Find(config, "filepath", filepath); //!< hostfxr.dllのパスの取得.
}

HostfxrModule::Settings HostfxrModule::Settings::ParseFromConfig(const Configuration& config) {
	Settings settings;

	if (!config.Contains(kConfigPath.GetPath())) {
		StreamLogger::Warning(
			"Script::HostfxrModule::Settings | config does not exist. path: {}", kConfigPath.GetPath()
		);
		return settings; //!< 設定が存在しない.
	}
	
	settings.Parse(config.GetConfig(Settings::kConfigPath.GetPath()));        //!< 全体設定の取得.
	settings.Parse(config.GetConfig(Settings::kConfigPath.GetProfilePath())); //!< プロファイル設定の取得.

	return settings;
}

void HostfxrModule::Settings::Log(const Settings& settings) {
	StreamLogger::Debug("Script::HostfxrModule::Settings | filepath: {}", settings.filepath.generic_string());
}

////////////////////////////////////////////////////////////////////////////////////////////
// HostfxrModule class methods
////////////////////////////////////////////////////////////////////////////////////////////

void HostfxrModule::Load(const Configuration& config) {

	settings_ = Settings::ParseFromConfig(config); //!< 設定の取得.
	Settings::Log(settings_); //!< 設定のログ.

	if (settings_.filepath.empty()) {
		StreamLogger::Info("Script::HostfxrModule | hostfxr.dll filepath is not set.");
		return; //!< hostfxr.dllのパスが設定されていない場合はロードしない.
	}

	//!< moduleの読み込み
	module_ = LoadLibraryW(settings_.filepath.generic_wstring().c_str());
	StreamLogger::Assert(module_ != nullptr, std::format("failed to load hostfxr library. filepath: {}", settings_.filepath.generic_string()));

	//!< hostfxrの関数ポインタの取得
	initFptr_        = HostfxrModule::GetFunction<hostfxr_initialize_for_runtime_config_fn>(module_, "hostfxr_initialize_for_runtime_config");
	getDelegateFptr_ = HostfxrModule::GetFunction<hostfxr_get_runtime_delegate_fn>(module_, "hostfxr_get_runtime_delegate");
	closeFptr_       = HostfxrModule::GetFunction<hostfxr_close_fn>(module_, "hostfxr_close");

	StreamLogger::Info("Script::HostfxrModule | loaded successfully.");
}

void HostfxrModule::Unload() {
	if (module_ != nullptr) {
		FreeLibrary(module_);
		module_ = nullptr;
	}

	initFptr_        = nullptr;
	getDelegateFptr_ = nullptr;
	closeFptr_       = nullptr;

	StreamLogger::Info("Script::HostfxrModule | unloaded successfully.");
}

hostfxr_handle HostfxrModule::CreateHandle(const std::filesystem::path& runtimeConfigPath) const {
	StreamLogger::Assert(module_ != nullptr, "hostfxr module is not loaded.");

	hostfxr_handle handle = nullptr;
	int32_t rc = initFptr_(
		runtimeConfigPath.generic_wstring().c_str(),
		nullptr,
		&handle
	);
	StreamLogger::Assert(rc == 0 && handle != nullptr, std::format("failed to initialize hostfxr handle."));

	return handle;
}

void HostfxrModule::CloseHandle(hostfxr_handle handle) const {
	StreamLogger::Assert(module_ != nullptr, "hostfxr module is not loaded.");

	if (handle == nullptr) {
		return; //!< handleがnullptrの場合は何もしない.
	}

	closeFptr_(handle);
}
