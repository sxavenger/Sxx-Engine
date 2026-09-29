#include "CameraCache.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [CameraCache] ProjectionData structure methods
////////////////////////////////////////////////////////////////////////////////////////////

CameraCache::ProjectionData CameraCache::ProjectionData::Transfer(const Matrix4x4f& projection) {
	ProjectionData data = {};
	data.projection        = projection;
	data.projectionInverse = Matrix4x4f::Inverse(projection);

	return data;
}

CameraCache::ProjectionData CameraCache::ProjectionData::Identity() noexcept {
	ProjectionData data = {};
	data.projection        = Matrix4x4f::Identity();
	data.projectionInverse = Matrix4x4f::Identity();

	return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CameraCache class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CameraCache::Cache(const World::CameraComponent& camera) {

	if (!projectionBuffer_.HasHandle()) { //!< projectionBufferが作成されていない場合
		//!< projectionの作成
		projectionBuffer_ = Graphics::Core::CreateConstantBuffer<ProjectionData>(
			Graphics::kFrameCount
		);
		
		projectionBuffer_.SetName(L"CameraCache");
	}

	//!< projectionの転送
	projectionBuffer_.At() = ProjectionData::Transfer(camera.GetProjectionMatrix());

}
