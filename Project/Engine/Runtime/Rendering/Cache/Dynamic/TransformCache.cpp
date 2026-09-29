#include "TransformCache.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [TransformCache] TransformationData structure
////////////////////////////////////////////////////////////////////////////////////////////

TransformCache::TransformationData TransformCache::TransformationData::Transfer(const Transformation3d& transformation) {
	TransformationData data = {};
	data.world        = Transformation3d::ConvertMatrix(transformation);
	data.worldInverse = Transformation3d::ConvertMatrix(Transformation3d::Inverse(transformation));

	return data;
}

TransformCache::TransformationData TransformCache::TransformationData::Identity() noexcept {
	TransformationData data = {};
	data.world        = Matrix4x4f::Identity();
	data.worldInverse = Matrix4x4f::Identity();

	return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
// TransformCache class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TransformCache::Cache(const World::TransformComponent& transform) {

	if (!transformationBuffer_.HasHandle()) { //!< transformationBufferが作成されていない場合
		//!< transformationの作成
		transformationBuffer_ = Graphics::Core::CreateConstantBuffer<TransformationData>(
			Graphics::kFrameCount
		);
		
		transformationBuffer_.SetName(L"TransformCache");
	}

	//!< transformationの転送
	transformationBuffer_.At() = TransformationData::Transfer(transform.GetTransformation());

}
