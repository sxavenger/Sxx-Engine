#include "SurfaceMaterialBuilder.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Assimp/AssimpCommon.h"
#include "../Assimp/AssimpImporter.h"
#include "../Assimp/AssimpMaterial.h"
#include "../Importer/TextureImporter.h"

//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Format/Json/JsonFile.h>

//* c++
#include <bitset>

////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceMaterialBuilder class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SurfaceMaterialBuilder::Build(std::shared_ptr<SurfaceMaterial>& material) {

	//!< ファイルパスの取得
	const std::filesystem::path& filepath = material->GetFilepath();

	if (!std::filesystem::exists(filepath)) {
		STREAM_LOG_ERROR("Assets::SurfaceMaterialBuilder | file does not exist. filepath: {}", filepath.generic_string());
		return; //!< ファイルが存在しない場合は処理を終了
	}

	//!< metadataの読み込み
	json::node node = JsonFile::Load(filepath);

	//!< metadataの設定
	const SurfaceMaterialMetadata& metadata = material->metadata_ = SurfaceMaterialMetadata::Deserialize(node["metadata"]);

	switch (metadata.GetType()) {
		case MetadataType::Reference:
			material->description_ = SurfaceMaterialBuilder::BuildReference(material->GetDirectory(), metadata.GetReferenceData());
			break;

		case MetadataType::Inline:
			material->description_ = {}; //!< TODO: Inline情報の定義.
			STREAM_LOG_ERROR("Assets::SurfaceMaterialBuilder | inline surface material metadata type is not defined. filepath: {}", filepath.generic_string());
			return;

		default:
			STREAM_LOG_ERROR("Assets::SurfaceMaterialBuilder | surface material metadata type is unknown. filepath: {}", filepath.generic_string());
			return; //!< metadataがUnknownの場合は処理を終了
	}

	STREAM_LOG_INFO("Assets::SurfaceMaterialBuilder | surface material build completed. name: {}", material->GetName());

}

SurfaceMaterial::Description SurfaceMaterialBuilder::BuildReference(const std::filesystem::path& directory, const SurfaceMaterialMetadata::ReferenceData& data) {

	//!< 参照されているmaterialのファイルパスの取得
	std::filesystem::path path = (directory / data.filepath).lexically_normal();

	//!< materialの取得
	const AssimpImporter importer = AssimpCommon::GetImporter(path);
	const AssimpMaterial material = importer.GetMaterial(data.index);

	//!< descriptionの作成
	SurfaceMaterial::Description description;

	//!< textureの設定

	description.textures[SurfaceMaterial::TextureType::Diffuse] = SurfaceMaterialBuilder::Unify(
		SurfaceMaterialBuilder::GetTextureId(material, directory, AssimpTextureType::Diffuse),
		SurfaceMaterialBuilder::GetTextureId(material, directory, AssimpTextureType::BaseColor)
	);

	description.textures[SurfaceMaterial::TextureType::Emissive]
		= SurfaceMaterialBuilder::GetTextureId(material, directory, AssimpTextureType::Emissive);

	description.textures[SurfaceMaterial::TextureType::Normal] = SurfaceMaterialBuilder::Unify(
		SurfaceMaterialBuilder::GetTextureId(material, directory, AssimpTextureType::Normals),
		SurfaceMaterialBuilder::GetTextureId(material, directory, AssimpTextureType::Height)
	);

	description.textures[SurfaceMaterial::TextureType::Roughness]
		= SurfaceMaterialBuilder::GetTextureId(material, directory, AssimpTextureType::Roughness);

	description.textures[SurfaceMaterial::TextureType::Metallic]
		= SurfaceMaterialBuilder::GetTextureId(material, directory, AssimpTextureType::Metalness);

	//!< transformの設定
	description.transform = material.GetTransform();

	//!< valueの設定

	description.diffuse   = material.GetDiffuse();
	description.emissive  = material.GetEmissive();
	description.roughness = material.GetRoughness();
	description.metallic  = material.GetMetallic();

	return description;
}

Graphics::ColorEncoding SurfaceMaterialBuilder::GetTextureEncoding(AssimpTextureType type) {
	switch (type) {
		case AssimpTextureType::Diffuse:
		case AssimpTextureType::BaseColor:
		case AssimpTextureType::Emissive:
			return Graphics::ColorEncoding::Lightness; //!< sRGB Color Space

		default:
			return Graphics::ColorEncoding::Intensity;
	}
}

std::optional<Uuid> SurfaceMaterialBuilder::GetTextureId(const AssimpMaterial& material, const std::filesystem::path& directory, AssimpTextureType type) {

	std::optional<std::filesystem::path> path = material.GetTexturePath(directory.parent_path(), type);

	if (!path.has_value()) {
		return std::nullopt; //!< テクスチャが存在しない場合はnulloptを返す
	}

	TextureImporter::ReferenceParameters parameters = {};
	parameters.encoding         = SurfaceMaterialBuilder::GetTextureEncoding(type);
	parameters.isGenerateMipmap = true;

	return TextureImporter::Import(path.value(), parameters);
}

std::optional<Uuid> SurfaceMaterialBuilder::Unify(const std::optional<Uuid>& lhs, const std::optional<Uuid>& rhs) {
	std::bitset<2> flags = {};
	flags[1] = lhs.has_value();
	flags[0] = rhs.has_value();

	switch (flags.to_ulong()) {
		case 0b01: //!< rhsのみ値を持つ場合
			return rhs;

		case 0b10: //!< lhsのみ値を持つ場合
			return lhs;

		case 0b11: //!< lhs, rhsが両方とも値を持つ場合
			STREAM_ASSERT(*lhs == *rhs, "Assets::SurfaceMaterialBuilder | uuid not equal. lhs: {}, rhs: {}", lhs->Serialize(), rhs->Serialize());
			return lhs; //!< lhsとrhsが同じ値を持つ場合はlhsを返す

		default:
			return std::nullopt;
	}
}
