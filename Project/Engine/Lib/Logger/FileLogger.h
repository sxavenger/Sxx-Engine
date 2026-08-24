#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* logger
#include "LoggerUtil.h"

//* c++
#include <mutex>
#include <filesystem>
#include <fstream>

////////////////////////////////////////////////////////////////////////////////////////////
// FileLogger class
////////////////////////////////////////////////////////////////////////////////////////////
class FileLogger final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mode enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Mode : std::ofstream::openmode {
		Truncate = std::ofstream::out | std::ofstream::trunc, //!< 既存のファイルを削除して新規作成するモード.
		Append   = std::ofstream::app | std::ofstream::app,   //!< 既存のファイルに追記するモード.
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	//* output methods *//

	//! @brief ログファイルに文字列を出力する.
	static void OutputA(const std::string_view& message);

	//! @brief ログファイルに文字列を出力する.
	static void OutputW(const std::wstring_view& message);

	//* formatter output methods *//

	template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
	static void OutputA(std::format_string<Args...> format, Args&&... args);

	template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
	static void OutputW(std::wformat_string<Args...> format, Args&&... args);

	//* separator methods *//

	//! @brief ログファイルに区切り線を出力する.
	static void SeparatorA(const char separator = '#', size_t len = 128);

	//! @brief ログファイルに区切り線を出力する.
	static void SeparatorW(const wchar_t separator = L'#', size_t len = 128);

	//* end line methods *//

	//! @brief ログファイルに空行を出力する.
	static void Endline();

	//* stamp output methods *//

	//! @brief コンソールにスタンプ付きの文字列を出力する.
	static void OutputStampA(const std::string_view& label, LoggerUtil::Level level, const TracePoint& point, const std::string_view& message);

	//! @brief コンソールにスタンプ付きの文字列を出力する.
	static void OutputStampW(const std::wstring_view& label, LoggerUtil::Level level, const TracePoint& point, const std::wstring_view& message);

	//* filename methods *//

	static std::filesystem::path CreateFilename(const std::filesystem::path& extension);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* thread *//

	static inline std::mutex mutex_ = {};

	//* file *//

	static inline const std::filesystem::path kDirectory = "Logs"; //!< ログファイルを格納するディレクトリのパス.
	static inline const std::filesystem::path kExtension = ".log"; //!< ログファイルの拡張子.
	static inline const std::filesystem::path kFilename  = FileLogger::CreateFilename(kExtension); //!< ログファイルのパス.

};

////////////////////////////////////////////////////////////////////////////////////////////
// FileLogger class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename... Args> requires (LoggerUtil::FormatA<Args>&& ...)
inline void FileLogger::OutputA(std::format_string<Args...> format, Args&&... args) {
	FileLogger::OutputA(std::format(format, std::forward<Args>(args)...));
}

template <typename... Args> requires (LoggerUtil::FormatW<Args>&& ...)
inline void FileLogger::OutputW(std::wformat_string<Args...> format, Args&&... args) {
	FileLogger::OutputW(std::format(format, std::forward<Args>(args)...));
}
