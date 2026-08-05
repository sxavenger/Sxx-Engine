#include "CrashHandler.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "StreamLogger.h"

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
	StreamLogger::Debug("CrashHandler | install unhandled exception filter.");

	StreamLogger::Info("CrashHandler | install exception filter.");
}

void CrashHandler::Uninstall() {
	if (handle_ != nullptr) {
		//!< ハンドルが存在する場合は、ハンドルを削除する
		RemoveVectoredExceptionHandler(handle_);
		handle_ = nullptr;
	}

	StreamLogger::Info("CrashHandler | uninstall exception filter.");
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

std::string_view CrashHandler::GetExceptionName(DWORD code) {
	switch (code) {
		case EXCEPTION_ACCESS_VIOLATION:
			return "EXCEPTION_ACCESS_VIOLATION";

		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";

		case EXCEPTION_BREAKPOINT:
			return "EXCEPTION_BREAKPOINT";

		case EXCEPTION_DATATYPE_MISALIGNMENT:
			return "EXCEPTION_DATATYPE_MISALIGNMENT";

		case EXCEPTION_FLT_DENORMAL_OPERAND:
			return "EXCEPTION_FLT_DENORMAL_OPERAND";

		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			return "EXCEPTION_FLT_DIVIDE_BY_ZERO";

		case EXCEPTION_FLT_OVERFLOW:
			return "EXCEPTION_FLT_OVERFLOW";

		case EXCEPTION_FLT_UNDERFLOW:
			return "EXCEPTION_FLT_UNDERFLOW";

		case EXCEPTION_ILLEGAL_INSTRUCTION:
			return "EXCEPTION_ILLEGAL_INSTRUCTION";

		case EXCEPTION_IN_PAGE_ERROR:
			return "EXCEPTION_IN_PAGE_ERROR";

		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			return "EXCEPTION_INT_DIVIDE_BY_ZERO";

		case EXCEPTION_INT_OVERFLOW:
			return "EXCEPTION_INT_OVERFLOW";

		case EXCEPTION_INVALID_HANDLE:
			return "EXCEPTION_INVALID_HANDLE";

		case EXCEPTION_STACK_OVERFLOW:
			return "EXCEPTION_STACK_OVERFLOW";

		default:
			return "UNKNOWN_EXCEPTION";
	}
}

LONG WINAPI CrashHandler::UnhandledExceptionFilterFunc(EXCEPTION_POINTERS* exception) {
	std::call_once(CrashHandler::once_, [&]() {
		StreamLogger::Critical("CrashHandler | unhandled exception filter called.");
		CrashHandler::CreateMinidump(exception);
	});

	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI CrashHandler::ExceptionFilterFunc(EXCEPTION_POINTERS* exception) {

	if (exception == nullptr || exception->ExceptionRecord == nullptr) {
		//!< 例外情報が不正な場合は、次のフィルタに処理を渡す
		StreamLogger::Critical("CrashHandler | exception filter called with invalid exception information.");
		return EXCEPTION_CONTINUE_SEARCH;
	}

	std::call_once(CrashHandler::once_, [&]() {
		StreamLogger::Critical("CrashHandler | exception filter called. Exception: {}", CrashHandler::GetExceptionName(exception->ExceptionRecord->ExceptionCode));
		CrashHandler::CreateMinidump(exception);
	});

	return EXCEPTION_CONTINUE_SEARCH;
}

