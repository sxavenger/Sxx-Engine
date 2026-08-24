#include "AssimpCommon.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>

//* assimp
#include <assimp/version.h>

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpCommon class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string AssimpCommon::GetAssimpVersion() {
	return std::format("{}.{}.{}", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());
}

const AssimpImporter& AssimpCommon::GetImporter(const std::filesystem::path& filepath) {

	std::filesystem::path path = filepath.lexically_normal(); //!< 正規化されたパスを使用する

	if (!cache_.contains(path)) {
		//!< キャッシュが存在しない場合は新規にimporterを作成してキャッシュに追加

		static const FlagEnum<AssimpOption> option //!< optionは共通の設定を使用.
			= AssimpOption::ConvertToLeftHanded
			| AssimpOption::Triangulate
			| AssimpOption::CalcTangentSpace
			| AssimpOption::JoinIdenticalVertices
			| AssimpOption::LimitBoneWeights
			| AssimpOption::SortByPrimitiveType
			| AssimpOption::FindDegenerates
			| AssimpOption::FindInvalidData;

		AssimpImporter importer = AssimpImporter::Load(path, option);
		cache_.emplace(path, importer);

		STREAM_LOG_DEBUG("Assets::AssimpCommon | assimp importer cached. filepath: {}", path.generic_string());
	}

	return cache_.at(path);
}

void AssimpCommon::ClearCache() {
	cache_.clear();
}
