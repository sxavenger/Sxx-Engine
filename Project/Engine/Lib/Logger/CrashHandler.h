#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "LoggerUtil.h"
#include "FileLogger.h"

//* lib
#include <Lib/CXXAttribute.hpp>

//* windows
#include <windows.h>

//* c++
#include <mutex>
#include <filesystem>

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

	//* break point methods *//

	NORETURN static void Breakpoint();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* file *//

	static inline const std::filesystem::path kDirectory = "Dumps"; //!< ダンプファイルを格納するディレクトリのパス.
	static inline const std::filesystem::path kExtension = ".dmp"; //!< ダンプファイルの拡張子.
	static inline const std::filesystem::path kFilename  = FileLogger::CreateFilename(kExtension); //!< ダンプファイルのパス.

	//* thread *//

	static inline std::once_flag once_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* handle helper methods *//

	static void CreateMinidump(EXCEPTION_POINTERS* exception);

	//* crash handler *//

	static LONG WINAPI UnhandledExceptionFilterFunc(EXCEPTION_POINTERS* exception);

};
