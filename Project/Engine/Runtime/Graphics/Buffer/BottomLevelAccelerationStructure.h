#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../GraphicsUtil.h"
#include "../Core/Device.h"
#include "../Core/GraphicsCommandContext.h"
#include "AccelerationStructure.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// BottomLevelAccelerationStructure class
////////////////////////////////////////////////////////////////////////////////////////////
class BottomLevelAccelerationStructure final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Geometry structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Geometry final {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		Geometry() noexcept = default;
		Geometry(const D3D12_RAYTRACING_GEOMETRY_DESC& desc) noexcept : desc(desc) {}

		//* desc option *//

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS GetInputs(bool isUpdate) const;

		//* static methods *//

		static Geometry CreateTriangleGeometry(
			D3D12_GPU_VIRTUAL_ADDRESS vertexBufferAddress, UINT64 vertexStride, UINT vertexCount,
			D3D12_GPU_VIRTUAL_ADDRESS indexBufferAddress, UINT indexCount
		);

		//=========================================================================================
		// public variables
		//=========================================================================================

		D3D12_RAYTRACING_GEOMETRY_DESC desc = {};

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief BottomLevelAccelerationStructureを構築する.
	void Build(
		const Device& device, const GraphicsCommandContext& context,
		const Geometry& geometry
	);

	//! @brief 既存のBottomLevelAccelerationStructureを更新する
	void Update(
		const GraphicsCommandContext& context,
		const Geometry& geometry
	);

	//! @brief BottomLevelAccelerationStructureのGPU仮想アドレスを取得する.
	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return accelerationStructure_.GetGpuVirtualAddress(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* Graphics *//

	AccelerationStructure accelerationStructure_;

};

SXAVENGER_ENGINE_NAMESPACE_END
