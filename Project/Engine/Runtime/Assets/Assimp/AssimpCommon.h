#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "AssimpOption.h"
#include "AssimpImporter.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Flag/Flag.h>

//* c++
#include <string>
#include <filesystem>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpCommon class
////////////////////////////////////////////////////////////////////////////////////////////
class AssimpCommon final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* version option *//

	//! @brief assimpのバージョンを取得する
	static std::string GetAssimpVersion();

	//* importer cache option *//

	//! @brief assimpのimporterを取得する
	//! @note キャッシュが存在する場合はキャッシュを返し, 存在しない場合は新規にimporterを作成して返す
	static const AssimpImporter& GetImporter(const std::filesystem::path& filepath);

	//! @brief assimpのimporterのキャッシュをクリアする
	static void ClearCache();

private:

	//=========================================================================================
	// public variables
	//=========================================================================================

	static inline std::unordered_map<std::filesystem::path, AssimpImporter> cache_; //!< assimpのimporterのキャッシュ

};

SXAVENGER_ENGINE_NAMESPACE_END
