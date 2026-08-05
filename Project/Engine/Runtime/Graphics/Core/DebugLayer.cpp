#include "DebugLayer.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Toml/TomlReader.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [DebugLayer] Settings structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DebugLayer::Settings::Parse(const toml::table& config) {
	TomlReader<bool>::Find(config, "enable", enable);                                     //!< デバッグレイヤーを有効にするかの取得.
	TomlReader<bool>::Find(config, "enableGpuBasedValidation", enableGpuBasedValidation); //!< GPUベースのバリデーションを有効にするかの取得.
}

DebugLayer::Settings DebugLayer::Settings::ParseFromConfig(const Configuration& config) {

	Settings settings;

	if (!config.Contains(kConfigPath.GetPath())) {
		StreamLogger::Warning(
			"Graphics::DebugLayer::Settings | config does not exist. path: {}", kConfigPath.GetPath()
		);
		return settings; //!< 設定が存在しない.
	}
	
	settings.Parse(config.GetConfig(Settings::kConfigPath.GetPath()));        //!< 全体設定の取得.
	settings.Parse(config.GetConfig(Settings::kConfigPath.GetProfilePath())); //!< プロファイル設定の取得.

	return settings;
}

void DebugLayer::Settings::Log(const Settings& settings) {
	StreamLogger::Debug("Graphics::DebugLayer::Settings | enable: {}", settings.enable);
	StreamLogger::Debug("Graphics::DebugLayer::Settings | enableGpuBasedValidation: {}", settings.enableGpuBasedValidation);
}

////////////////////////////////////////////////////////////////////////////////////////////
// DebugLayer class methods
////////////////////////////////////////////////////////////////////////////////////////////

DebugLayer::~DebugLayer() {
	StreamLogger::Info("Graphics::DebugLayer | debug layer terminated.");
}

void DebugLayer::Init(const Configuration& config) {

	settings_ = Settings::ParseFromConfig(config); //!< 設定の取得.
	Settings::Log(settings_); //!< 設定のログ.

	layer_ = DebugLayer::CreateDebugLayer(); //!< DebugLayerの生成

	if (layer_ == nullptr) {
		StreamLogger::Error("Graphics::DebugLayer | failed to create debug layer.");
		return;
	}

#ifdef DEVELOPMENT
	if (settings_.enable) {
		layer_->EnableDebugLayer(); //!< デバッグレイヤーの有効化.
	}

	if (settings_.enableGpuBasedValidation) {
		layer_->SetEnableGPUBasedValidation(true); //!< GPUベースのバリデーションの有効化.
	}
#endif // DEVELOPMENT

	StreamLogger::Info("Graphics::DebugLayer | successfully initialized debug layer.");
}

ComPtr<ID3D12Debug1> DebugLayer::CreateDebugLayer() {

	ComPtr<ID3D12Debug1> debug;

	//!< デバッグレイヤーの取得
	auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));
	if (FAILED(hr)) {
		StreamLogger::Error(
			std::format(
				L"Graphics::DebugLayer | failed to get debug interface. _com_error: {}",
				ComPtrUtil::GetComErrorMessage(hr)
			)
		);

		return nullptr;
	}

	return debug;
}
