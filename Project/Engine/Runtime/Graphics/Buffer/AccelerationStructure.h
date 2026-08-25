#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Core/Device.h"
#include "../Core/GraphicsCommandContext.h"
#include "Resource.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// AccelerationStructure structure
////////////////////////////////////////////////////////////////////////////////////////////
struct AccelerationStructure final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief AccelerationStructureを構築する.
	void Build(
		const GraphicsCommandContext& context,
		const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs
	);

	//! @brief AccelerationStructureを更新する.
	//! @pre AccelerationStructureを構築する際にD3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATEフラグを指定
	//! @warning 更新を行う場合は, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATEフラグを指定する必要がある.
	void Update(
		const GraphicsCommandContext& context,
		const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs
	);

	//! @brief AccelerationStructureのGPU仮想アドレスを取得する.
	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const;

	//* static methods *//

	//! @brief AccelerationStructureを作成する.
	static AccelerationStructure Create(
		const Device& device,
		const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs
	);

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* Graphics *//

	Resource buffer;
	Resource scratch;

	// HACK: Resource共に, ResourceHandleとして保持しておく必要がある.

	//* flags *//

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	static Resource CreateAccelerationStructureBuffer(
		const Device& device,
		const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO& info
	);

	static Resource CreateScratchBuffer(
		const Device& device,
		const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO& info
	);

};

SXAVENGER_ENGINE_NAMESPACE_END
