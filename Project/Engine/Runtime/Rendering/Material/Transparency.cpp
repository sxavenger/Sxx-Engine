#include "Transparency.h"
SXAVENGER_ENGINE_USING_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// Transparency structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Material::Transparency Material::Transparency::FromValue(float _transparency) noexcept {
	Transparency transparency;
	transparency.operation    = Operation::Transparency;
	transparency.transparency = _transparency;
	transparency.textureIndex = NULL;

	return transparency;
}

Material::Transparency Material::Transparency::FromTexture(const Graphics::Descriptor::Handle& _handle) noexcept {
	Transparency transparency;
	transparency.operation    = Operation::Texture;
	transparency.transparency = 0.0f;
	transparency.textureIndex = _handle.GetIndex();

	return transparency;
}
