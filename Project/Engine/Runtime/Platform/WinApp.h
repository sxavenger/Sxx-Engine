#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* windows
#include <windows.h>

//* mf
#include <mfapi.h>

//* c++
#include <filesystem>
#include <optional>

//-----------------------------------------------------------------------------------------
// pragma comment
//-----------------------------------------------------------------------------------------
//* windows
#pragma comment(lib, "winmm.lib")

//* mf
#pragma comment(lib, "mfplat.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Platform)

////////////////////////////////////////////////////////////////////////////////////////////
// WinApp class
////////////////////////////////////////////////////////////////////////////////////////////
class WinApp final {
public:

	//=========================================================================================
	// public methods
	//========================================================================================

	static void Init();

	static void Term();

	//* windows api option *//

	//! @brief Dialogを表示して保存するファイル名を取得する.
	//! @param[in] title   Dialogのタイトル
	//! @param[in] current Dialogの初期表示ファイル名
	//! @param[in] filter Dialogのフィルタ設定 {表示名, 拡張子}
	//! @param[in] extension 何も指定されなかった場合に付与する拡張子
	//! @retval std::filesystem::path 保存するファイル名
	//! @retval std::nullopt          キャンセルされた場合
	static std::optional<std::filesystem::path> GetSaveFilepath(
		const std::wstring& title,
		const std::filesystem::path& current,
		const std::pair<std::wstring, std::wstring>& filter,
		const std::filesystem::path& extension
	);

	//! @brief Dialogを表示して開くファイル名を取得する.
	//! @param[in] title  Dialogのタイトル
	//! @param[in] current Dialogの初期表示ファイル名
	//! @param[in] filter Dialogのフィルタ設定 {表示名, 拡張子}
	//! @retval std::filesystem::path 開くファイル名
	//! @retval std::nullopt          キャンセルされた場合
	static std::optional<std::filesystem::path> GetOpenFilepath(
		const std::wstring& title,
		const std::filesystem::path& current,
		const std::pair<std::wstring, std::wstring>& filter
	);

	//! @brief エクスプローラーで指定したファイルを開く.
	//! @param[in] filepath ファイルパス
	//! @retval true  成功
	//! @retval false 失敗
	static bool OpenExplorer(const std::filesystem::path& filepath);

	//! @brief 指定したファイルを関連付けられたアプリケーションで開く.
	//! @param[in] filepath ファイルパス
	//! @retval true  成功
	//! @retval false 失敗
	static bool OpenApplication(const std::filesystem::path& filepath);

	//! @brief クリップボードに文字列を設定する.
	//! @param text 設定する文字列
	//! @rerval true  成功
	//! @rerval false 失敗
	static bool SetClipboardText(const std::wstring_view& text);

	//! @brief クリップボードから文字列を取得する.
	//! @retval std::wstring 取得した文字列
	//! @retval std::nullopt 取得に失敗した場合
	static std::optional<std::wstring> GetClipboardText();

};

SXAVENGER_ENGINE_NAMESPACE_END
