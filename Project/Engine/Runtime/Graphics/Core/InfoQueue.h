#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "Device.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Configuration/Configuration.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// InfoQueue class
////////////////////////////////////////////////////////////////////////////////////////////
class InfoQueue final {
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

		static inline constexpr Configuration::Path kConfigPath{ "Graphics.InfoQueue" }; //!< 設定のパス.

		std::vector<D3D12_MESSAGE_SEVERITY> breakSeverity = { D3D12_MESSAGE_SEVERITY_CORRUPTION, D3D12_MESSAGE_SEVERITY_ERROR }; //!< デバッグブレークするメッセージの重大度.
		std::vector<D3D12_MESSAGE_SEVERITY> filterSeverity = { D3D12_MESSAGE_SEVERITY_INFO }; //!< フィルタリングするメッセージの重大度.
		std::vector<D3D12_MESSAGE_ID> filterId = {}; //!< フィルタリングするメッセージのID.

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	InfoQueue() noexcept = default;
	~InfoQueue();

	void Init(const Configuration& config, const Device& device);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12InfoQueue> infoQueue_;

	//* settings *//

	Settings settings_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* initailize helper methods *//

	static ComPtr<ID3D12InfoQueue> CreateInfoQueue(RefPtr<ID3D12Device8> device);

	//* settings helper methods *//

	void ApplySettings();

};

SXAVENGER_ENGINE_NAMESPACE_END
