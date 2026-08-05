#include "AssetStorage.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Scheduler/System.h>

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonFile.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

AssetStorage* AssetStorage::GetInstance() {
	static AssetStorage instance;
	return &instance;
}

BaseAssetMetadata AssetStorage::LoadMetadata(const std::filesystem::path& filepath) {
	StreamLogger::Assert(
		std::filesystem::exists(filepath),
		std::format("asset file does not exist. filepath: {}", filepath.generic_string())
	); //!< ファイルが存在しない場合は例外を投げる

	//!< Assetファイルの読み込み
	json::node node = JsonFile::Load(filepath);

	return BaseAssetMetadata::Deserialize(filepath, node); //!< metadataの取得
}

void AssetStorage::PushBuildTask(const std::shared_ptr<BaseAsset>& asset, const Scheduler::CpuTask::Function& function) {
	Scheduler::System::PushCpuTask(
		std::format("build asset: {}", asset->GetName()),
		function,
		static_cast<uint8_t>(Scheduler::TaskPriority::Default),
		asset->GetStatePointer()
	);
}
