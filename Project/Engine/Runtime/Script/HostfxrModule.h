#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* hostfxr
#include <dotnet/hostfxr.h>
#include <dotnet/coreclr_delegates.h>

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Configuration/Configuration.h>

//* lib
#include <Lib/Traits/Concept.h>
#include <Lib/Logger/StreamLogger.h>

//* windows
#include <windows.h>

//* c++
#include <filesystem>
#include <string>
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Script)

////////////////////////////////////////////////////////////////////////////////////////////
// HostfxrModule class
////////////////////////////////////////////////////////////////////////////////////////////
class HostfxrModule final {
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

		static inline constexpr Configuration::Path kConfigPath{ "Script.HostfxrModule" }; //!< 設定のパス.

		std::filesystem::path filepath; //!< hostfxr.dllのパス.

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* module option *//

	void Load(const Configuration& config);

	void Unload();

	//* function pointer option *//

	hostfxr_handle CreateHandle(const std::filesystem::path& runtimeConfigPath) const;

	void CloseHandle(hostfxr_handle handle) const;

	template <typename T>
	T GetDelegateFunction(hostfxr_delegate_type type, hostfxr_handle handle) const;

	//* operator [comparison] <std::nullptr_t> *//

	bool operator==(std::nullptr_t) const noexcept { return module_ == nullptr; }
	bool operator!=(std::nullptr_t) const noexcept { return module_ != nullptr; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* module handle *//

	HMODULE module_ = nullptr;

	//* function pointers *//

	hostfxr_initialize_for_runtime_config_fn initFptr_ = nullptr;
	hostfxr_close_fn closeFptr_                        = nullptr;
	hostfxr_get_runtime_delegate_fn getDelegateFptr_   = nullptr;

	//* settings *//

	Settings settings_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	template <Concept::FunctionPointer T>
	static T GetFunction(HMODULE module, const std::string_view& name);

};

////////////////////////////////////////////////////////////////////////////////////////////
// HostfxrModule class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Concept::FunctionPointer T>
inline T HostfxrModule::GetFunction(HMODULE module, const std::string_view& name) {
	T function = reinterpret_cast<T>(GetProcAddress(module, name.data()));

	StreamLogger::Assert(function != nullptr, std::format("failed to get hostfxr function pointer: {}", name));
	return function;
}

template <typename T>
inline T HostfxrModule::GetDelegateFunction(hostfxr_delegate_type type, hostfxr_handle handle) const {
	StreamLogger::Assert(module_ != nullptr, "hostfxr module is not loaded.");

	T function = nullptr;
	int32_t rc = getDelegateFptr_(handle, type, reinterpret_cast<void**>(&function));
	StreamLogger::Assert(rc == 0 && function != nullptr, std::format("failed to get hostfxr delegate function pointer: {}", type));

	return function;
}

SXAVENGER_ENGINE_NAMESPACE_END
