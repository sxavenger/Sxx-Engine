#include "SurfaceMaterial.h"
SXAVENGER_ENGINE_USING_(Assets)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Reflection/EnumUtil.h>
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [SurfaceMaterial] Description structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Uuid SurfaceMaterial::Description::GetTextureUuid(TextureType type) const {
	STREAM_ASSERT(textures.At(type).has_value(), "surface material description does not have texture. type: {}", type);
	return textures.At(type).value();
}

////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceMaterial class methods
////////////////////////////////////////////////////////////////////////////////////////////

const SurfaceMaterial::Description& SurfaceMaterial::GetDescription() const {
	Scheduler::TaskHandle handle = BaseAsset::GetTaskHandle();

	if (handle != Scheduler::TaskState::State::Completed) {
		STREAM_LOG_WARNING("Assets::SurfaceMaterial | surface material is not ready. name: {}, state: {}", GetName(), handle.GetState());
		handle.Wait(); //!< materialが準備できるまで待機
	}

	return description_;
}
