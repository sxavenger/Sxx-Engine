#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "LoggerUtil.h"

//* windows
#include <windows.h>

//* c++
#include <filesystem>
#include <mutex>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// CrashHandler class
////////////////////////////////////////////////////////////////////////////////////////////
class CrashHandler final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Install();

	static void Uninstall();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* file *//

	static inline const std::filesystem::path kDirectory = "Dumps";
	static inline const std::filesystem::path kFilename  = LoggerUtil::CreateFilename(".dmp");

	//* thread *//

	static inline std::once_flag once_ = {};

	//* handle *//

	static inline PVOID handle_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* handle helper methods *//

	static void CreateMinidump(EXCEPTION_POINTERS* exception);

	static std::string_view GetExceptionName(DWORD code);

	//* crash handler *//

	static LONG WINAPI UnhandledExceptionFilterFunc(EXCEPTION_POINTERS* exception);

	static LONG WINAPI ExceptionFilterFunc(EXCEPTION_POINTERS* exception);

};
