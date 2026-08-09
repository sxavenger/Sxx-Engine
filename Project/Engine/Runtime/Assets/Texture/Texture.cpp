#include "Texture.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Logger/StreamLogger.h>
#include <Lib/Reflection/EnumUtil.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [Texture] Description structure methods
////////////////////////////////////////////////////////////////////////////////////////////

uint16_t Texture::Description::GetDepthOrArraySize() const {
	return std::max(depth, arraySize); //!< depthとarraySizeの大きい方を返す. (基本どちらか一方しか使用しないため)
}

bool Texture::Description::IsCubemap() const {
	return (miscflags[0] & DirectX::TEX_MISC_TEXTURECUBE) != 0;
}

bool Texture::Description::IsArray() const {
	return arraySize > (IsCubemap() ? 6 : 1); //!< cubemapの場合は6以上, それ以外は1以上でarray扱い
}

Graphics::ColorEncoding Texture::Description::GetColorEncoding() const {
	return Graphics::GetColorEncoding(format);
}

D3D12_SRV_DIMENSION Texture::Description::GetSRVDimension() const {

	switch (dimension) {
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			return IsArray()
				? D3D12_SRV_DIMENSION_TEXTURE1DARRAY //!< arraySizeが1より大きい場合はTexture1DArrayとして扱う
				: D3D12_SRV_DIMENSION_TEXTURE1D;     //!< arraySizeが1の場合はTexture1Dとして扱う

		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			if (IsCubemap()) {
				return IsArray()
					? D3D12_SRV_DIMENSION_TEXTURECUBEARRAY //!< arraySizeが6より大きい場合はTextureCubeArrayとして扱う
					: D3D12_SRV_DIMENSION_TEXTURECUBE;     //!< arraySizeが6の場合はTextureCubeとして扱う
			}

			return IsArray()
				? D3D12_SRV_DIMENSION_TEXTURE2DARRAY //!< arraySizeが1より大きい場合はTexture2DArrayとして扱う
				: D3D12_SRV_DIMENSION_TEXTURE2D;     //!< arraySizeが1の場合はTexture2Dとして扱う

		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			return D3D12_SRV_DIMENSION_TEXTURE3D;

		default:
			StreamLogger::Error("Texture::Description | unknown texture resource dimension. dimension: {}", dimension);
			return D3D12_SRV_DIMENSION_UNKNOWN;
	}
}

Texture::Description Texture::Description::Parse(const DirectX::TexMetadata & metadata) {
	Description description = {};
	description.dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	description.size      = { static_cast<uint32_t>(metadata.width), static_cast<uint32_t>(metadata.height) };
	description.depth     = static_cast<uint16_t>(metadata.depth);
	description.arraySize = static_cast<uint16_t>(metadata.arraySize);
	description.miplevels = static_cast<uint16_t>(metadata.mipLevels);
	description.format    = static_cast<DXGI_FORMAT>(metadata.format);
	description.miscflags = { metadata.miscFlags, metadata.miscFlags2 };

	return description;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class methods
////////////////////////////////////////////////////////////////////////////////////////////

const DirectX::ScratchImage& Texture::GetImage() const {
	Scheduler::TaskHandle handle = BaseAsset::GetTaskHandle();

	if (handle != Scheduler::TaskState::State::Completed) {
		StreamLogger::Warning("Asset::Texture | texture image is not ready. name: {}, state: {}", GetName(), handle.GetState());
		handle.Wait(); //!< imageが準備できるまで待機
	}

	return image_;
}

const Texture::Description& Texture::GetDescription() const {
	Scheduler::TaskHandle handle = BaseAsset::GetTaskHandle();

	if (handle != Scheduler::TaskState::State::Completed) {
		StreamLogger::Warning("Asset::Texture | texture description is not ready. name: {}, state: {}", GetName(), handle.GetState());
		handle.Wait(); //!< descriptionが準備できるまで待機
	}

	return description_;
}
