#include "InfoQueue.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Format/Toml/TomlReader.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [InfoQueue] Settings structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void InfoQueue::Settings::Parse(const toml::table& config) {
	TomlReader<D3D12_MESSAGE_SEVERITY>::FindVector(config, "breakSeverity", breakSeverity);   //!< デバッグブレークするメッセージの重大度の取得.
	TomlReader<D3D12_MESSAGE_SEVERITY>::FindVector(config, "filterSeverity", filterSeverity); //!< フィルタリングするメッセージの重大度の取得.
	TomlReader<D3D12_MESSAGE_ID>::FindVector(config, "filterId", filterId);                   //!< フィルタリングするメッセージのIDの取得.
}

InfoQueue::Settings InfoQueue::Settings::ParseFromConfig(const Configuration& config) {
	Settings settings;

	if (!config.Contains(kConfigPath.GetPath())) {
		STREAM_LOG_WARNING(
			"Graphics::InfoQueue::Settings | config does not exist. path: {}", kConfigPath.GetPath()
		);
		return settings; //!< 設定が存在しない.
	}

	settings.Parse(config.GetConfig(Settings::kConfigPath.GetPath()));        //!< 全体設定の取得.
	settings.Parse(config.GetConfig(Settings::kConfigPath.GetProfilePath())); //!< プロファイル設定の取得.

	return settings;
}

void InfoQueue::Settings::Log(const Settings& settings) {
	{
		std::stringstream ss;
		ss << "Graphics::InfoQueue::Settings | breakSeverity: [";
		for (size_t i = 0; i < settings.breakSeverity.size(); i++) {
			ss << EnumUtil<D3D12_MESSAGE_SEVERITY>::GetName(settings.breakSeverity.at(i));
			if (i < settings.breakSeverity.size() - 1) {
				ss << ", ";
			}
		}
		ss << "]";
		STREAM_LOG_DEBUG(ss.str());
	}

	{
		std::stringstream ss;
		ss << "Graphics::InfoQueue::Settings | filterSeverity: [";
		for (size_t i = 0; i < settings.filterSeverity.size(); i++) {
			ss << EnumUtil<D3D12_MESSAGE_SEVERITY>::GetName(settings.filterSeverity.at(i));
			if (i < settings.filterSeverity.size() - 1) {
				ss << ", ";
			}
		}
		ss << "]";
		STREAM_LOG_DEBUG(ss.str());
	}

	{
		std::stringstream ss;
		ss << "Graphics::InfoQueue::Settings | filterId: [";
		for (size_t i = 0; i < settings.filterId.size(); i++) {
			ss << EnumUtil<D3D12_MESSAGE_ID>::GetName(settings.filterId.at(i));
			if (i < settings.filterId.size() - 1) {
				ss << ", ";
			}
		}
		ss << "]";
		STREAM_LOG_DEBUG(ss.str());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// InfoQueue class methods
////////////////////////////////////////////////////////////////////////////////////////////

InfoQueue::~InfoQueue() {
	STREAM_LOG_INFO("Graphics::InfoQueue | info queue terminated.");
}

void InfoQueue::Init(const Configuration& config, const Device& device) {

	settings_ = Settings::ParseFromConfig(config); //!< 設定の取得.
	Settings::Log(settings_); //!< 設定のログ.

	infoQueue_ = InfoQueue::CreateInfoQueue(device.GetDevice()); //!< InfoQueueのインターフェースの取得.

	if (infoQueue_ == nullptr) {
		STREAM_LOG_WARNING("Graphics::InfoQueue | failed to create info queue.");
		return; //!< InfoQueueのインターフェースの取得に失敗.
	}

	ApplySettings(); //!< 設定の適用.

	STREAM_LOG_INFO("Graphics::InfoQueue | successfully initialized info queue.");
}

ComPtr<ID3D12InfoQueue> InfoQueue::CreateInfoQueue(RefPtr<ID3D12Device8> device) {

	ComPtr<ID3D12InfoQueue> infoQueue;

	auto hr = device->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf())); //!< InfoQueueのインターフェースの取得.
	if (FAILED(hr)) {
		STREAM_LOG_ERROR(
			L"Graphics::InfoQueue | failed to get info queue interface. _com_error: {}",
			ComPtrUtil::GetComErrorMessage(hr)
		);

		return nullptr;
	}

	return infoQueue;
}

void InfoQueue::ApplySettings() {

	for (const auto& severity : settings_.breakSeverity) {
		infoQueue_->SetBreakOnSeverity(severity, true); //!< デバッグブレークするメッセージの重大度の設定.
	}

	D3D12_INFO_QUEUE_FILTER filter = {};

	//!< フィルタリングするメッセージの重大度のリストの設定.
	filter.DenyList.pSeverityList = settings_.filterSeverity.data(); 
	filter.DenyList.NumSeverities = static_cast<UINT>(settings_.filterSeverity.size());

	//!< フィルタリングするメッセージのIDのリストの設定.
	filter.DenyList.pIDList = settings_.filterId.data();
	filter.DenyList.NumIDs  = static_cast<UINT>(settings_.filterId.size());

	infoQueue_->PushStorageFilter(&filter); //!< フィルタの適用.

}
