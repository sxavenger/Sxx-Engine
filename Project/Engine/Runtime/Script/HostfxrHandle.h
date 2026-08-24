#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* script
#include "HostfxrModule.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Core/Configuration/Configuration.h>

//* lib
#include <Lib/Traits/Concept.h>
#include <Lib/Logger/StreamLogger.h>

//* c++
#include <filesystem>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Script)

////////////////////////////////////////////////////////////////////////////////////////////
// HostfxrHandle class
////////////////////////////////////////////////////////////////////////////////////////////
class HostfxrHandle final {
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

		static inline constexpr Configuration::Path kConfigPath{ "Script.HostfxrHandle" }; //!< 設定のパス.

		std::filesystem::path runtimeConfigPath; //!< runtimeconfig.jsonのパス.

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================
	
	//* handle option *//

	void Init(const Configuration& config, const HostfxrModule& module);

	void Shutdown(const HostfxrModule& module);

	//* delegate function pointer option *//

	template <Concept::FunctionPointer T>
	T GetFunctionPointer(const std::filesystem::path& _assembly, const std::wstring_view& _namespace, const std::wstring_view& _class, const std::wstring& _function) const;

	//* operator [comparison] <std::nullptr_t> *//

	bool operator==(std::nullptr_t) const noexcept { return handle_ == nullptr; }
	bool operator!=(std::nullptr_t) const noexcept { return handle_ != nullptr; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* hostfxr handle *//

	hostfxr_handle handle_ = nullptr;

	//* delegate function pointer *//

	load_assembly_and_get_function_pointer_fn loadAssemblyAndGetFunctionPointerFptr_ = nullptr;

	//* settings *//

	Settings settings_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static std::wstring GetTypeName(
		const std::wstring_view& _namespace, const std::wstring_view& _class, const std::wstring_view& _assembly
	);

};

////////////////////////////////////////////////////////////////////////////////////////////
// HostfxrHandle class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Concept::FunctionPointer T>
inline T HostfxrHandle::GetFunctionPointer(const std::filesystem::path& _assembly, const std::wstring_view& _namespace, const std::wstring_view& _class, const std::wstring& _function) const {


	std::wstring stem = _assembly.stem().generic_wstring();
	std::wstring name = HostfxrHandle::GetTypeName(_namespace, _class, stem);

	T function = nullptr;
	int32_t rc = loadAssemblyAndGetFunctionPointerFptr_(
		_assembly.generic_wstring().c_str(),
		name.c_str(),
		_function.c_str(),
		UNMANAGEDCALLERSONLY_METHOD,
		nullptr,
		reinterpret_cast<void**>(&function)
	);
	STREAM_ASSERT(
		rc == 0 && function != nullptr,
		L"failed to get hostfxr delegate function pointer. assembly: {}, function: {}::{}.{}", _assembly.generic_wstring(), _namespace, _class, _function
	);

	return function;
}

SXAVENGER_ENGINE_NAMESPACE_END
