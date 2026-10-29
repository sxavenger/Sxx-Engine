#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "../Cache/Static/StaticMeshCache.h"
#include "../Cache/Static/SurfaceMaterialCache.h"
#include "../Cache/Dynamic/TransformCache.h"
#include "../Cache/Dynamic/DirectionalLightCache.h"
#include "../Cache/Dynamic/PointLightCache.h"

//* engine
#include <Runtime/Foundation.hpp>
#include <Runtime/Graphics/Command/GraphicsCommandContext.h>
#include <Runtime/Graphics/Buffer/TopLevelAccelerationStructure.h>
#include <Runtime/Scheduler/Common/TaskHandle.h>

//* c++
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// SceneProxy structure
////////////////////////////////////////////////////////////////////////////////////////////
struct SceneProxy final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// StaticMeshInstance structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct StaticMeshInstance {
	public:
	
		//=========================================================================================
		// public methods
		//=========================================================================================
	
		std::shared_ptr<const StaticMeshCache> mesh;
		std::shared_ptr<const SurfaceMaterialCache> material;
		std::shared_ptr<const TransformCache> transform;
	
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// DirectionalLightInstance structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct DirectionalLightInstance {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		std::shared_ptr<const DirectionalLightCache> light;
		std::shared_ptr<const TransformCache> transform;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// PointLightInstance structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PointLightInstance {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		std::shared_ptr<const PointLightCache> light;
		std::shared_ptr<const TransformCache> transform;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Setup();

	void Reset();

	void AddMeshInstance(const StaticMeshInstance& instance);

	void AddLightInstance(const DirectionalLightInstance& instance);

	void AddLightInstance(const PointLightInstance& instance);

	const Graphics::TopLevelAccelerationStructure& GetTopLevelAccelerationStructure() const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::list<StaticMeshInstance> staticMeshes;

	std::list<DirectionalLightInstance> directionalLights;
	std::list<PointLightInstance> pointLights;

	Graphics::TopLevelAccelerationStructure tlas;
	Scheduler::TaskHandle handle;

};

SXAVENGER_ENGINE_NAMESPACE_END
