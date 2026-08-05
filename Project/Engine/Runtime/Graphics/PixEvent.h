#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "GraphicsUtil.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Configuration/Configuration.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// PixEvent class
////////////////////////////////////////////////////////////////////////////////////////////
class PixEvent final {
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

		static inline constexpr Configuration::Path kConfigPath{ "Graphics.PixEvent" }; //!< 設定のパス.

		bool enable = false; //!< pixを有効にするか.

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// HUDOptions enum class [pix3.h | PIXHUDOptions参照]
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class HUDOptions : uint8_t {
		ShowOnAllWindows       = 0x1,
		ShowOnTargetWindowOnly = 0x2,
		ShowOnNoWindows        = 0x4
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init(const Configuration& config);

	static void BeginEvent(ID3D12GraphicsCommandList* commandList, const std::wstring_view& name, uint8_t indent);

	static void EndEvent(ID3D12GraphicsCommandList* commandList);

	static void CaptureNextFrames(const std::filesystem::path& filepath, uint32_t frames);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline std::filesystem::path directory_; //!< [pix3.h] pixのlibファイルのDirectory. (WinPix.exeが存在する場所を指定する)

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* pix helper methods *//

	//! @brief [pix3.h] pixのlibファイルのDirectoryを探索する. 
	//! @note WinPix.exeが存在するversionのdirectoryを探す
	static std::filesystem::path FindPixDirectory();

	//! @brief [pix3.h] pixのlibファイルを読み込む.
	static void LoadPixModule(const std::filesystem::path& directory, const std::filesystem::path& filename);

	//* pix3 implementation methods *//

	static void* GetGpuCaptureFunctionPointer(const std::string& function);

	static HRESULT WINAPI PIXSetHUDOptionsImpl(HUDOptions options);

	static HRESULT WINAPI PIXGpuCaptureNextFramesImpl(const std::filesystem::path& filepath, uint32_t frames);

};

SXAVENGER_ENGINE_NAMESPACE_END
