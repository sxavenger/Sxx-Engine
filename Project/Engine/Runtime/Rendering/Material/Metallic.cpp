#include "Metallic.h"
SXAVENGER_ENGINE_USING_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// Metallic structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Material::Metallic Material::Metallic::FromValue(float _value) noexcept {
	Metallic metallic;
	metallic.operation      = Operation::Value;
	metallic.value          = _value;
	metallic.textureIndex   = NULL;
	metallic.textureChannel = Graphics::TextureChannel::G;

	return metallic;
}

Material::Metallic Material::Metallic::FromTexture(const Graphics::Descriptor::Handle& _handle, Graphics::TextureChannel _channel) noexcept {
	Metallic metallic;
	metallic.operation      = Operation::Texture;
	metallic.value          = 0.0f;
	metallic.textureIndex   = _handle.GetIndex();
	metallic.textureChannel = _channel;

	return metallic;
}
