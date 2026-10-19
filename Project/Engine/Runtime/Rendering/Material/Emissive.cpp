#include "Emissive.h"
SXAVENGER_ENGINE_USING_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// Emissive structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Material::Emissive Material::Emissive::FromColor(const Color3f& _color, float _intensity) noexcept {
	Emissive emissive;
	emissive.operation    = Operation::Color;
	emissive.color        = _color;
	emissive.textureIndex = NULL;
	emissive.intensity    = _intensity;

	return emissive;
}

Material::Emissive Material::Emissive::FromTexture(const Graphics::Descriptor::Handle& _handle, float _intensity) noexcept {
	Emissive emissive;
	emissive.operation    = Operation::Texture;
	emissive.color        = Color3f{};
	emissive.textureIndex = _handle.GetIndex();
	emissive.intensity    = _intensity;

	return emissive;
}
