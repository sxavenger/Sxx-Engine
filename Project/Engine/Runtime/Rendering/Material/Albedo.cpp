#include "Albedo.h"
SXAVENGER_ENGINE_USING_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// Albedo structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Material::Albedo Material::Albedo::FromColor(const Color3f& _color) noexcept {
	Albedo albedo;
	albedo.operation    = Operation::Color;
	albedo.color        = _color;
	albedo.textureIndex = NULL;

	return albedo;
}

Material::Albedo Material::Albedo::FromTexture(const Graphics::Descriptor::Handle& _handle) noexcept {
	Albedo albedo;
	albedo.operation    = Operation::Texture;
	albedo.color        = Color3f{};
	albedo.textureIndex = _handle.GetIndex();;

	return albedo;
}
