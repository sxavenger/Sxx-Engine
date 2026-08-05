#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Configuration/Configuration.h>

//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// DebugLayer class
////////////////////////////////////////////////////////////////////////////////////////////
class DebugLayer final {
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

		static inline constexpr Configuration::Path kConfigPath{ "Graphics.DebugLayer" }; //!< 設定のパス.

		bool enable                   = false; //!< デバッグレイヤーを有効にするか.
		bool enableGpuBasedValidation = false; //!< GPUベースのバリデーションを有効にするか.

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	DebugLayer() noexcept = default;
	~DebugLayer();

	//* debug layer option *//

	void Init(const Configuration& config);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Debug1> layer_;

	//* settings *//

	Settings settings_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* initailize helper methods *//

	static ComPtr<ID3D12Debug1> CreateDebugLayer();

};

SXAVENGER_ENGINE_NAMESPACE_END
