#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Buffer/ConstantBuffer.h>
#include <Runtime/World/Component/Transform/TransformComponent.h>

//* lib
#include <Lib/Math/Matrix4x4.h>
#include <Lib/Math/Transform/Transformation3d.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// TransformCache class
////////////////////////////////////////////////////////////////////////////////////////////
class TransformCache final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// TransformationData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct TransformationData {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		static TransformationData Transfer(const Transformation3d& transformation);

		//* constant value methods *//

		static TransformationData Identity() noexcept;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Matrix4x4f world;
		Matrix4x4f worldInverse;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Cache(const World::TransformComponent& transform);

	//* cache option *//

	const Graphics::ConstantBuffer<TransformationData>& GetTransformationBuffer() const { return transformationBuffer_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* projection *//

	Graphics::ConstantBuffer<TransformationData> transformationBuffer_;

	//!< ComponentのCache方法を考える.
};

SXAVENGER_ENGINE_NAMESPACE_END
