#include "Device.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Format/Toml/TomlReader.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [Device] Settings structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Device::Settings::Parse(const toml::table& config) {
	TomlReader<bool>::Find(config, "allowTearing", allowTearing);                   //!< ティアリングを許可するかの取得.
	TomlReader<D3D_SHADER_MODEL>::Find(config, "shaderModelTier", shaderModelTier); //!< Shader Modelの機能レベルの取得.
}

Device::Settings Device::Settings::ParseFromConfig(const Configuration& config) {

	Settings settings;

	if (!config.Contains(kConfigPath.GetPath())) {
		StreamLogger::Warning(
			"Graphics::Device::Settings | config does not exist. path: {}", kConfigPath.GetPath()
		);
		return settings; //!< 設定が存在しない.
	}

	settings.Parse(config.GetConfig(Settings::kConfigPath.GetPath()));        //!< 全体設定の取得.
	settings.Parse(config.GetConfig(Settings::kConfigPath.GetProfilePath())); //!< プロファイル設定の取得.

	//!< 制約の確認.
	StreamLogger::Assert(
		settings.shaderModelTier >= kRequireShaderModel,
		std::format("shader model tier is lower than required. shaderModelTier: {}", settings.shaderModelTier)
	);

	return settings;
}

void Device::Settings::Log(const Settings& settings) {
	StreamLogger::Debug("Graphics::Device::Settings | allowTearing: {}", settings.allowTearing);
	StreamLogger::Debug("Graphics::Device::Settings | shaderModelTier: {}", settings.shaderModelTier);
}

////////////////////////////////////////////////////////////////////////////////////////////
// [Device] Support structure methods
////////////////////////////////////////////////////////////////////////////////////////////

bool Device::Support::CheckAllowTearing(RefPtr<IDXGIFactory7> factory) {
	BOOL isSupport = false;
	auto hr = factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &isSupport, sizeof(BOOL));
	ComPtrUtil::Assert(hr, L"check feature tearing support error.");

	if (isSupport) {
		StreamLogger::Info("Graphics::Device::Support | tearing is allowed.");

	} else {
		StreamLogger::Warning("Graphics::Device::Support | tearing is not supported, so tearing is not allowed.");
	}

	return isSupport;
}

D3D_SHADER_MODEL Device::Support::CheckShaderModel(RefPtr<ID3D12Device8> device) {
	//!< Shader Modelの機能レベルの確認.
	D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_HIGHEST_SHADER_MODEL };
	auto hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(D3D12_FEATURE_DATA_SHADER_MODEL));
	ComPtrUtil::Assert(hr, L"check feature shader model support error.");

	StreamLogger::Info("Graphics::Device::Support | supported highest shader model tier: {}", shaderModel.HighestShaderModel);
	return shaderModel.HighestShaderModel;
}

bool Device::Support::CheckMeshShader(RefPtr<ID3D12Device8> device) {

	bool isSupport = false;

	D3D12_FEATURE_DATA_D3D12_OPTIONS7 features = {};
	auto hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS7));
	ComPtrUtil::Assert(hr, L"check feature mesh shader support error.");

	isSupport = (features.MeshShaderTier != D3D12_MESH_SHADER_TIER_NOT_SUPPORTED);

	if (isSupport) {
		StreamLogger::Info("Graphics::Device::Support | mesh shader is supported.");

	} else {
		StreamLogger::Warning("Graphics::Device::Support | mesh shader is not supported.");
	}

	return isSupport;
}

D3D12_RAYTRACING_TIER Device::Support::CheckRaytracing(RefPtr<ID3D12Device8> device) {
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 option = {};
	auto hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &option, sizeof(option));
	ComPtrUtil::Assert(hr, L"check feature raytracing support error.");

	StreamLogger::Info("Graphics::Device::Support | supported raytracing tier: {}", option.RaytracingTier);
	return option.RaytracingTier;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Device class methods
////////////////////////////////////////////////////////////////////////////////////////////

Device::~Device() {
	StreamLogger::Info("Graphics::Device | device terminated.");
}

void Device::Init(const Configuration& config) {

	//!< 設定の取得.
	settings_ = Settings::ParseFromConfig(config);
	Settings::Log(settings_); //!< 設定のログ.

	//!< DirectXの初期化.
	factory_ = Device::CreateFactory();
	adapter_ = Device::CreateAdapter(factory_.Get());
	device_  = Device::CreateDevice(adapter_.Get());

	//!< 設定の適用.
	CheckSupport();

	StreamLogger::Info("Graphics::Device | initialization complete.");
}

DXGI_QUERY_VIDEO_MEMORY_INFO Device::GetVideoMemoryInfo() const {
	DXGI_QUERY_VIDEO_MEMORY_INFO info = {};
	auto hr = adapter_->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info);
	ComPtrUtil::Assert(hr, L"query video memory info failed.");

	return info;
}

Device::Status Device::CheckDeviceStatus() const {
	auto hr = device_->GetDeviceRemovedReason();
	if (SUCCEEDED(hr)) {
		return Device::Status::Default; //!< デバイスは正常.
	}

	StreamLogger::Critical(
		L"Graphics::Device | device removed. _com_error_: {}", ComPtrUtil::GetComErrorMessage(hr)
	);

	return Device::Status::Removed; //!< デバイスは異常.
}

std::string_view Device::GetFutureLevel(D3D_FEATURE_LEVEL level) {
	switch (level) {
		case D3D_FEATURE_LEVEL_12_2: return "12.2";
		case D3D_FEATURE_LEVEL_12_1: return "12.1";
		case D3D_FEATURE_LEVEL_12_0: return "12.0";
		default: return "unknown";
	}
}

ComPtr<IDXGIFactory7> Device::CreateFactory() {
	ComPtr<IDXGIFactory7> factory;

	auto hr = CreateDXGIFactory(IID_PPV_ARGS(factory.GetAddressOf()));
	ComPtrUtil::Assert(hr, L"dxgi factory create failed.");

	return factory;
}

ComPtr<IDXGIAdapter4> Device::CreateAdapter(RefPtr<IDXGIFactory7> factory) {
	ComPtr<IDXGIAdapter4> useAdapter;

	for (UINT i = 0;; ++i) {

		ComPtr<IDXGIAdapter4> adapter;

		//!< アダプタの列挙
		auto hr = factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.GetAddressOf()));
		if (hr == DXGI_ERROR_NOT_FOUND) {
			break; //!< アダプタが見つからない.
		}
		ComPtrUtil::Assert(hr, L"dxgi adapter enum failed.");

		//!< アダプタ情報の取得
		DXGI_ADAPTER_DESC3 desc = {};
		hr = adapter->GetDesc3(&desc);
		ComPtrUtil::Assert(hr, L"dxgi adapter get desc failed.");

		if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) {
			continue; //!< ソフトウェアアダプタはスキップ.
		}

		//!< 使用するアダプタを保存.
		useAdapter = adapter;
		StreamLogger::Info(L"Graphics::Device | Use Adapter: {}", desc.Description);
		
		break;
	}

	StreamLogger::Assert(useAdapter != nullptr, "dxgi adapter not found.");
	return useAdapter;
}

ComPtr<ID3D12Device8> Device::CreateDevice(RefPtr<IDXGIAdapter4> adapter) {

	ComPtr<ID3D12Device8> device;

	static const D3D_FEATURE_LEVEL levels[] = { //!< デバイス生成の際に確認する機能レベルのリスト.
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};

	for (size_t i = 0; i < _countof(levels); ++i) {
		auto hr = D3D12CreateDevice(adapter.Get(), levels[i], IID_PPV_ARGS(device.GetAddressOf()));
		if (SUCCEEDED(hr)) {
			StreamLogger::Info("Graphics::Device | feature level: {}", Device::GetFutureLevel(levels[i]));
			break; //!< デバイスの生成に成功したらループを抜ける.
		}
	}

	StreamLogger::Assert(device != nullptr, "d3d12 device create failed.");
	return device;
}

void Device::CheckSupport() {

	//!< ティアリングの許可の設定.
	if (settings_.allowTearing) {
		support_.tearing = Support::CheckAllowTearing(factory_.Get());
	}

	//!< Shader Modelの機能レベルの確認.
	support_.shaderModel = Support::CheckShaderModel(device_.Get());
	StreamLogger::Assert(
		support_.shaderModel >= settings_.shaderModelTier,
		std::format("shader model tier is not supported. supported shader model: {}", support_.shaderModel)
	);

	//!< Mesh Shaderのサポートの確認.
	support_.meshShader = Support::CheckMeshShader(device_.Get());
	StreamLogger::Assert(
		support_.meshShader,
		"mesh shader is not supported."
	);

	//!< Raytracingの機能レベルの確認.
	support_.raytracingTier = Support::CheckRaytracing(device_.Get());
	StreamLogger::Assert(
		support_.raytracingTier >= D3D12_RAYTRACING_TIER_1_0,
		std::format("raytracing tier is not supported. supported raytracing tier: {}", support_.raytracingTier)
	);

}
