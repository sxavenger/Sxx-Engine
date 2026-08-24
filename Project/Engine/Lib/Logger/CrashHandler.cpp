#include "CrashHandler.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "ConsoleLogger.h"

//* windows
#include <dbghelp.h>

//-----------------------------------------------------------------------------------------
// pragma comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dbghelp.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// CrashHandler class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CrashHandler::Install() {
	SetUnhandledExceptionFilter(CrashHandler::UnhandledExceptionFilterFunc);
}

void CrashHandler::Uninstall() {
}

NORETURN void CrashHandler::Breakpoint() {
	__debugbreak(); //!< windowsのデバッガを起動する.
	// TODO: Dumpファイルを作成するようにする.
}

void CrashHandler::CreateMinidump(EXCEPTION_POINTERS* exception) {

	if (!std::filesystem::exists(kDirectory)) {
		//!< directoryが存在しない場合は作成する
		std::filesystem::create_directories(kDirectory);
	}

	const std::filesystem::path kFilepath = kDirectory / kFilename;

	HANDLE file = CreateFileW(
		kFilepath.generic_wstring().c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	if (file != INVALID_HANDLE_VALUE) {

		MINIDUMP_EXCEPTION_INFORMATION information = {};
		information.ThreadId          = GetCurrentThreadId();
		information.ExceptionPointers = exception;
		information.ClientPointers    = FALSE;

		const uint32_t type
			= MiniDumpWithDataSegs
			| MiniDumpWithThreadInfo
			| MiniDumpWithIndirectlyReferencedMemory;

		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			file,
			static_cast<MINIDUMP_TYPE>(type),
			&information,
			nullptr,
			nullptr
		);

		CloseHandle(file);
	}
}

LONG WINAPI CrashHandler::UnhandledExceptionFilterFunc(EXCEPTION_POINTERS* exception) {
	std::call_once(CrashHandler::once_, [&]() {
		CrashHandler::CreateMinidump(exception);
	});

	return EXCEPTION_EXECUTE_HANDLER;
}
