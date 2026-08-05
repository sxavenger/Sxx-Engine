#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Configuration/Configuration.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// Device class
////////////////////////////////////////////////////////////////////////////////////////////
class Device final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Settings structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Settings {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Parse(const toml::table& config);

		static Settings ParseFromConfig(const Configuration& config);

		static void Log(const Settings& settings);

		//=========================================================================================
		// public variables
		//=========================================================================================

		static inline constexpr Configuration::Path kConfigPath{ "Graphics.Device" }; //!< 設定のパス.

		bool allowTearing                = false; //!< ティアリングを許可するか.
		D3D_SHADER_MODEL shaderModelTier = kRequireShaderModel; //!< Shader Modelの機能レベル.

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Support structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Support {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* support helper methods *//

		static bool CheckAllowTearing(RefPtr<IDXGIFactory7> factory);

		static D3D_SHADER_MODEL CheckShaderModel(RefPtr<ID3D12Device8> device);

		static bool CheckMeshShader(RefPtr<ID3D12Device8> device);

		static D3D12_RAYTRACING_TIER CheckRaytracing(RefPtr<ID3D12Device8> device);

		//=========================================================================================
		// public variables
		//=========================================================================================

		bool tearing                         = false; //!< ティアリングがサポートされているか.
		D3D_SHADER_MODEL shaderModel         = D3D_SHADER_MODEL_NONE; //!< サポートされているShader Modelの機能レベル.
		bool meshShader                      = false; //!< Mesh Shaderがサポートされているか.
		D3D12_RAYTRACING_TIER raytracingTier = D3D12_RAYTRACING_TIER_NOT_SUPPORTED; //!< サポートされているRaytracingの機能レベル.

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Status : bool {
		Default, //!< デバイスの状態は正常.
		Removed, //!< デバイスが削除された.
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	Device() noexcept = default;
	~Device();

	void Init(const Configuration& config);

	//* factory option *//

	RefPtr<IDXGIFactory7> GetFactory() const { return factory_.Get(); }

	//* adapter option *//

	DXGI_QUERY_VIDEO_MEMORY_INFO GetVideoMemoryInfo() const;

	RefPtr<IDXGIAdapter4> GetAdapter() const { return adapter_.Get(); }

	//* device option *//

	//! @brief デバイスの状態を確認する.
	Device::Status CheckDeviceStatus() const;

	RefPtr<ID3D12Device8> GetDevice() const { return device_.Get(); }

	//* setting option *//

	const Settings& GetSettings() const { return settings_; }

	//* support option *//

	const Support& GetSupport() const { return support_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<IDXGIFactory7> factory_;
	ComPtr<IDXGIAdapter4> adapter_;

	ComPtr<ID3D12Device8> device_;

	//* status *//

	Settings settings_;
	Support  support_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* format helper methods *//

	static std::string_view GetFutureLevel(D3D_FEATURE_LEVEL level);

	//* initailize helper methods *//

	static ComPtr<IDXGIFactory7> CreateFactory();

	static ComPtr<IDXGIAdapter4> CreateAdapter(RefPtr<IDXGIFactory7> factory);

	static ComPtr<ID3D12Device8> CreateDevice(RefPtr<IDXGIAdapter4> adapter);

	//* support helper methods *//

	void CheckSupport();

};

SXAVENGER_ENGINE_NAMESPACE_END
