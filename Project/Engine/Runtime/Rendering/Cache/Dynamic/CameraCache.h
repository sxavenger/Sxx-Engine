#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* cache
#include "BaseDynamicCache.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/ConstantBuffer.h>
#include <Runtime/World/Component/Camera/CameraComponent.h>

//* lib
#include <Lib/Math/Matrix4x4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// CameraCache class
////////////////////////////////////////////////////////////////////////////////////////////
class CameraCache final
	: public BaseDynamicCache {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ProjectionData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ProjectionData {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		static ProjectionData Identity() noexcept;

		static ProjectionData Transfer(const Matrix4x4f& projection);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Matrix4x4f projection;
		Matrix4x4f projectionInverse;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	CameraCache() noexcept  = default;
	~CameraCache() override = default;

	//* cache option *//

	void Cache(const World::CameraComponent& camera);

	const Graphics::ConstantBuffer<ProjectionData>& GetProjectionBuffer() const { return projectionBuffer_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* projection *//

	Graphics::ConstantBuffer<ProjectionData> projectionBuffer_;

};

SXAVENGER_ENGINE_NAMESPACE_END
