#include "HostfxrHandle.h"
SXAVENGER_ENGINE_USING_(Script)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Toml/TomlReader.h>

//* c++
#include <sstream>
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// [HostfxrHandle] Settings structure methods 
////////////////////////////////////////////////////////////////////////////////////////////

void HostfxrHandle::Settings::Parse(const toml::table& config) {
	TomlReader<std::filesystem::path>::Find(config, "runtimeConfigPath", runtimeConfigPath); //!< runtimeconfig.jsonのパスの取得.
}

HostfxrHandle::Settings HostfxrHandle::Settings::ParseFromConfig(const Configuration& config) {
	Settings settings;

	if (!config.Contains(kConfigPath.GetPath())) {
		StreamLogger::Warning(
			"Script::HostfxrHandle::Settings | config does not exist. path: {}", kConfigPath.GetPath()
		);
		return settings; //!< 設定が存在しない.
	}
	
	settings.Parse(config.GetConfig(Settings::kConfigPath.GetPath()));        //!< 全体設定の取得.
	settings.Parse(config.GetConfig(Settings::kConfigPath.GetProfilePath())); //!< プロファイル設定の取得.

	return settings;
}

void HostfxrHandle::Settings::Log(const Settings& settings) {
	StreamLogger::Debug("Script::HostfxrHandle::Settings | runtimeConfigPath: {}", settings.runtimeConfigPath.generic_string());
}

////////////////////////////////////////////////////////////////////////////////////////////
// HostfxrHandle class methods
////////////////////////////////////////////////////////////////////////////////////////////

void HostfxrHandle::Init(const Configuration& config, const HostfxrModule& module) {

	settings_ = Settings::ParseFromConfig(config); //!< 設定の取得.
	Settings::Log(settings_); //!< 設定のログ.

	if (settings_.runtimeConfigPath.empty()) {
		StreamLogger::Info("Script::HostfxrHandle | runtimeconfig.json filepath is not set.");
		return; //!< runtimeconfig.jsonのパスが設定されていない場合は初期化しない.
	}

	//!< handleの初期化
	handle_ = module.CreateHandle(settings_.runtimeConfigPath);

	//!< 関数ポインタの取得
	loadAssemblyAndGetFunctionPointerFptr_ = module.GetDelegateFunction<load_assembly_and_get_function_pointer_fn>(hdt_load_assembly_and_get_function_pointer, handle_);

	StreamLogger::Info("Script::HostfxrHandle | initialized successfully.");
}

void HostfxrHandle::Shutdown(const HostfxrModule& module) {
	if (handle_ != nullptr) {
		module.CloseHandle(handle_);
		handle_ = nullptr;
	}

	loadAssemblyAndGetFunctionPointerFptr_ = nullptr;

	StreamLogger::Info("Script::HostfxrHandle | shutdown successfully.");
}

std::wstring HostfxrHandle::GetTypeName(const std::wstring_view& _namespace, const std::wstring_view& _class, const std::wstring_view& _assembly) {
	std::wstringstream stream;

	if (!_namespace.empty()) {
		stream << _namespace << L".";
	}

	stream << _class;

	return std::format(L"{}, {}", stream.str(), _assembly);
}
