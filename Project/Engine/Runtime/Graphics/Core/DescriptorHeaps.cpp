#include "DescriptorHeaps.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Format/Toml/TomlReader.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [DescriptorHeaps] Settings structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DescriptorHeaps::Settings::Parse(const toml::table& config) {
	TomlReader<UINT>::Find(config, "descriptorCapacityRTV", descriptorCapacityRTV);                 //!< RTVのデスクリプタ数
	TomlReader<UINT>::Find(config, "descriptorCapacityDSV", descriptorCapacityDSV);                 //!< DSVのデスクリプタ数
	TomlReader<UINT>::Find(config, "descriptorCapacityCBV_SRV_UAV", descriptorCapacityCBV_SRV_UAV); //!< CBV/SRV/UAVのデスクリプタ数
}

DescriptorHeaps::Settings DescriptorHeaps::Settings::ParseFromConfig(const Configuration& config) {

	Settings settings;

	if (!config.Contains(DescriptorHeaps::Settings::kConfigPath.GetPath())) {
		StreamLogger::Warning(
			"Graphics::DescriptorHeaps::Settings | config does not exist. path: {}", DescriptorHeaps::Settings::kConfigPath.GetPath()
		);

		return settings;
	}

	settings.Parse(config.GetConfig(DescriptorHeaps::Settings::kConfigPath.GetPath()));
	settings.Parse(config.GetConfig(DescriptorHeaps::Settings::kConfigPath.GetProfilePath()));

	return settings;
}

void DescriptorHeaps::Settings::Log(const Settings& settings) {
	StreamLogger::Debug("Graphics::DescriptorHeaps::Settings | descriptorCapacityRTV: {}", settings.descriptorCapacityRTV);
	StreamLogger::Debug("Graphics::DescriptorHeaps::Settings | descriptorCapacityDSV: {}", settings.descriptorCapacityDSV);
	StreamLogger::Debug("Graphics::DescriptorHeaps::Settings | descriptorCapacityCBV_SRV_UAV: {}", settings.descriptorCapacityCBV_SRV_UAV);
}

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorHeaps class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DescriptorHeaps::Init(const Configuration& config, const Device& device) {

	settings_ = Settings::ParseFromConfig(config); //!< 設定の読み込み.
	Settings::Log(settings_); //!< 設定のログ出力.

	//* descriptor poolの初期化 *//

	pools_[EnumUtil<DescriptorCategory>::Cast(DescriptorCategory::RTV)].Init(device, DescriptorCategory::RTV, settings_.descriptorCapacityRTV);                         //!< RTVのdescriptor poolの初期化.
	pools_[EnumUtil<DescriptorCategory>::Cast(DescriptorCategory::DSV)].Init(device, DescriptorCategory::DSV, settings_.descriptorCapacityDSV);                         //!< DSVのdescriptor poolの初期化.
	pools_[EnumUtil<DescriptorCategory>::Cast(DescriptorCategory::SRV_CBV_UAV)].Init(device, DescriptorCategory::SRV_CBV_UAV, settings_.descriptorCapacityCBV_SRV_UAV); //!< CBV/SRV/UAVのdescriptor poolの初期化.

	StreamLogger::Info("Graphics::DescriptorHeaps | initialization complete.");

}

NODISCARD Descriptor DescriptorHeaps::Allocate(DescriptorCategory category) {
	return pools_[EnumUtil<DescriptorCategory>::Cast(category)].Allocate(); //!< categoryに対応するdescriptor poolからdescriptorを割り当てる.
}

DescriptorAllocator& DescriptorHeaps::GetAllocator(DescriptorCategory category) {
	return pools_[EnumUtil<DescriptorCategory>::Cast(category)]; //!< categoryに対応するdescriptor poolの参照を返す.
}

const DescriptorAllocator& DescriptorHeaps::GetAllocator(DescriptorCategory category) const {
	return pools_[EnumUtil<DescriptorCategory>::Cast(category)]; //!< categoryに対応するdescriptor poolのconst参照を返す.
}
