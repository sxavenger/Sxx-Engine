#include "Roughness.h"
SXAVENGER_ENGINE_USING_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// Roughness structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Material::Roughness Material::Roughness::FromValue(float _value) noexcept {
	Roughness roughness;
	roughness.operation      = Operation::Value;
	roughness.value          = _value;
	roughness.textureIndex   = NULL;
	roughness.textureChannel = Graphics::TextureChannel::G;

	return roughness;
}

Material::Roughness Material::Roughness::FromTexture(const Graphics::Descriptor::Handle& _handle, Graphics::TextureChannel _channel) noexcept {
	Roughness roughness;
	roughness.operation      = Operation::Texture;
	roughness.value          = 0.0f;
	roughness.textureIndex   = _handle.GetIndex();
	roughness.textureChannel = _channel;

	return roughness;
}
