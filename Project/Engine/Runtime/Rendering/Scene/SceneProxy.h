#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* rendering
#include "../Cache/Static/StaticMeshCache.h"
#include "../Cache/Static/SurfaceMaterialCache.h"
#include "../Cache/Dynamic/TransformCache.h"

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

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Setup();

	void Reset();

	void AddInstance(const StaticMeshInstance& instance);

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::list<StaticMeshInstance> staticMeshes;

	Graphics::TopLevelAccelerationStructure tlas;
	Scheduler::TaskHandle handle;

};

SXAVENGER_ENGINE_NAMESPACE_END
