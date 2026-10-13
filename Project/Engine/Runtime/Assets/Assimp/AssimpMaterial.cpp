#include "AssimpMaterial.h"
SXAVENGER_ENGINE_USING_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpMaterial class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string_view AssimpMaterial::GetName() const {
	const aiString& name = material_->GetName();
	return std::string_view(name.data, name.length);
}

std::optional<std::filesystem::path> AssimpMaterial::GetTexturePath(const std::filesystem::path& directory, AssimpTextureType type, uint32_t index) const {

	if (material_->GetTextureCount(static_cast<aiTextureType>(type)) == 0) {
		return std::nullopt; //!< テクスチャが存在しない場合はnulloptを返す
	}

	aiString part;
	material_->GetTexture(static_cast<aiTextureType>(type), index, &part);

	std::filesystem::path filepath = directory / part.C_Str();
	return filepath.lexically_normal();
}

Transform2d AssimpMaterial::GetTransform() const {

	aiUVTransform t;
	if (material_->Get(AI_MATKEY_UVTRANSFORM(aiTextureType_DIFFUSE, 0), t) != AI_SUCCESS) {
		return Transform2d{}; //!< 取得できなかった場合はデフォルト値を返す
	}

	Transform2d transform;
	transform.translation = { t.mTranslation.x, t.mTranslation.y };
	transform.rotation    = t.mRotation;
	transform.scale       = { t.mScaling.x, t.mScaling.y };

	return transform;
}

Color3f AssimpMaterial::GetDiffuse() const {

	aiColor3D color;
	if (material_->Get(AI_MATKEY_COLOR_DIFFUSE, color) != AI_SUCCESS) {
		return Color3f{}; //!< 取得できなかった場合はデフォルト値を返す
	}

	return Color3f{ color.r, color.g, color.b };
	
}

Color3f AssimpMaterial::GetEmissive() const {

	aiColor3D color;
	if (material_->Get(AI_MATKEY_COLOR_EMISSIVE, color) != AI_SUCCESS) {
		return Color3f{}; //!< 取得できなかった場合はデフォルト値を返す
	}

	return Color3f{ color.r, color.g, color.b };
	
}

float AssimpMaterial::GetRoughness() const {

	ai_real roughness;
	if (material_->Get(AI_MATKEY_SHININESS, roughness) != AI_SUCCESS) {
		return 0.0f; //!< 取得できなかった場合はデフォルト値を返す
	}

	return static_cast<float>(roughness);
	
}

float AssimpMaterial::GetMetallic() const {

	ai_real metallic;
	if (material_->Get(AI_MATKEY_METALLIC_FACTOR, metallic) != AI_SUCCESS) {
		return 0.0f; //!< 取得できなかった場合はデフォルト値を返す
	}

	return static_cast<float>(metallic);

}
