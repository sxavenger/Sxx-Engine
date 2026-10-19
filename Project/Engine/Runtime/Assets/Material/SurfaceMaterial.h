#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/BaseAsset.h"
#include "SurfaceMaterialMetadata.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Container/ArrayEnum.h>
#include <Lib/Math/Color3.h>
#include <Lib/Math/Transform/Transform2d.h>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceMaterial class
////////////////////////////////////////////////////////////////////////////////////////////
class SurfaceMaterial final
	: public BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class TextureType : uint8_t {
		Diffuse,
		Emissive,
		Normal,
		Roughness,
		Metallic,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Description structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Description {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		bool HasTexture(TextureType type) const { return textures.At(type).has_value(); }

		Uuid GetTextureUuid(TextureType type) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		ArrayEnum<std::optional<Uuid>, EnumUtil<TextureType>::GetCount(), TextureType> textures; //!< 各テクスチャのuuidを保持する配列. TextureTypeのenum順に格納する.

		Transform2d transform;

		Color3f diffuse;
		Color3f emissive;
		float roughness;
		float metallic;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	SurfaceMaterial(const BaseAssetMetadata& metadata) noexcept : BaseAsset(metadata) {}
	~SurfaceMaterial() noexcept override = default;

	//* description option *//

	const Description& GetDescription() const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* directory *//

	static inline const std::filesystem::path kDirectory = L"Materials"; //!< materialの保存ディレクトリ

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* metadata *//

	SurfaceMaterialMetadata metadata_;

	//* description *//

	Description description_;

	//-----------------------------------------------------------------------------------------
	// friend class
	//-----------------------------------------------------------------------------------------
	friend class SurfaceMaterialBuilder;

};

SXAVENGER_ENGINE_NAMESPACE_END
