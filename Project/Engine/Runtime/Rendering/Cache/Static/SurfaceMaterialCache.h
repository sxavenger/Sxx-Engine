#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cache
#include "BaseStaticCache.h"
#include "TextureCache.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Assets/Material/SurfaceMaterial.h>
#include <Runtime/Rendering/Material/SurfaceMaterialBuffer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceMaterialCache class
////////////////////////////////////////////////////////////////////////////////////////////
class SurfaceMaterialCache final
	: public BaseStaticCache {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Cache(const std::shared_ptr<const Assets::SurfaceMaterial>& material);

	void Refresh() override;

	//* cache option *//

	const SurfaceMaterialBuffer& GetMaterialBuffer() const { return materialBuffer_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<const Assets::SurfaceMaterial> material_;

	//* Material Buffer *//

	SurfaceMaterialBuffer materialBuffer_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static bool HasTextureCache(const Uuid& uuid);

	static std::shared_ptr<const TextureCache> GetTextureCache(const Uuid& uuid);

	static SurfaceMaterialBuffer CreateSurfaceMaterialBuffer(const std::string_view& name);

};

SXAVENGER_ENGINE_NAMESPACE_END
