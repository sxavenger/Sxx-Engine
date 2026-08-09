#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/BaseAsset.h"
#include "TextureMetadata.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Vector2.h>

//* DirectXTex
#include <DirectXTex/DirectXTex.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class
////////////////////////////////////////////////////////////////////////////////////////////
class Texture final
	: public BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Description structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Description {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* description option *//

		uint16_t GetDepthOrArraySize() const;

		bool IsCubemap() const;

		bool IsArray() const;

		Graphics::ColorEncoding GetColorEncoding() const;

		D3D12_SRV_DIMENSION GetSRVDimension() const;

		//* parse option *//

		static Description Parse(const DirectX::TexMetadata& metadata);

		//=========================================================================================
		// public variables
		//=========================================================================================

		D3D12_RESOURCE_DIMENSION dimension;
		Vector2ui size;
		uint16_t depth;
		uint16_t arraySize;
		uint16_t miplevels;
		DXGI_FORMAT format;
		std::array<uint32_t, 2> miscflags;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	Texture(const BaseAssetMetadata& metadata) noexcept : BaseAsset(metadata) {}
	~Texture() noexcept override = default;

	//* metadata option *//

	//* texture image option *//

	const DirectX::ScratchImage& GetImage() const;

	const Description& GetDescription() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* metadata *//

	TextureMetadata metadata_;

	//* texture image *//

	DirectX::ScratchImage image_;
	Description description_;

	//# 参考
	// - [UE5 Texture Asset](https://dev.epicgames.com/documentation/unreal-engine/texture-asset-editor-in-unreal-engine)

	//-----------------------------------------------------------------------------------------
	// friend class
	//-----------------------------------------------------------------------------------------
	friend class TextureBuilder;

};

SXAVENGER_ENGINE_NAMESPACE_END
