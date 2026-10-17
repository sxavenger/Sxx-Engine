#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Device/Device.h"
#include "ResourceAllocator.h"
#include "AccelerationStructure.h"
#include "BottomLevelAccelerationStructure.h"
#include "DimensionBuffer.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Flag/Flag.h>
#include <Lib/Math/Matrix4x4.h>

//* c++
#include <bitset>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// TopLevelAccelerationStructure class
////////////////////////////////////////////////////////////////////////////////////////////
class TopLevelAccelerationStructure final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Instance structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Instance {
	public:
		
		//=========================================================================================
		// public variables
		//=========================================================================================

		const BottomLevelAccelerationStructure* blas     = nullptr;
		FlagEnum< D3D12_RAYTRACING_INSTANCE_FLAGS> flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		UINT instanceId                                  = 0;
		std::bitset<8> instanceMask                      = 0xFF;
		
		Matrix4x4f transform = Matrix4x4f::Identity();
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// BuildMode enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class BuildMode : bool {
		Build  = false,
		Update = true
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* instance option *//

	void ResetInstance();

	void AddInstance(const Instance& instance);

	//* desc option *//

	void SetupInstanceDesc(const Device& device, const GraphicsCommandContext& context, ResourceAllocator& allocator);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::list<Instance> instances_;
	uint32_t previousInstanceCount_ = 0; //!< 1frame前のintance数.

	DimensionBuffer<D3D12_RAYTRACING_INSTANCE_DESC> descs_;
	AccelerationStructure accelerationStructure_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	BuildMode PrepareInstanceDesc(const Device& device, ResourceAllocator& allocator);

	void Build(const Device& device, const GraphicsCommandContext& context, ResourceAllocator& allocator);

	void Update(const Device& device, const GraphicsCommandContext& context);

};

SXAVENGER_ENGINE_NAMESPACE_END
