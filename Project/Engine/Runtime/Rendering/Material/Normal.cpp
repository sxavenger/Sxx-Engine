#include "Normal.h"
SXAVENGER_ENGINE_USING_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// Normal structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Material::Normal Material::Normal::FromValue() noexcept {
	Normal normal;
	normal.operation    = Operation::Value;
	normal.textureIndex = NULL;

	return normal;
}

Material::Normal Material::Normal::FromTexture(const Graphics::Descriptor::Handle& _handle) noexcept {
	Normal normal;
	normal.operation    = Operation::Texture;
	normal.textureIndex = _handle.GetIndex();

	return normal;
}
