#include "AccelerationStructure.h"
SXAVENGER_ENGINE_USING_(Graphics)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "ResourceDesc.h"

//* lib
#include <Lib/Logger/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AccelerationStructure structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void AccelerationStructure::Build(
	const GraphicsCommandContext& context,
	const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs) {

	//!< descの設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
	desc.Inputs                           = inputs;
	desc.ScratchAccelerationStructureData = scratch.GetGpuVirtualAddress();
	desc.DestAccelerationStructureData    = buffer.GetGpuVirtualAddress();

	//!< 実行
	context.GetCommandList()->BuildRaytracingAccelerationStructure(
		&desc,
		0, nullptr
	);

	// todo: acceleration structureの圧縮が必要な場合はここで行う.
	// 参考文献: [bufferの圧縮](https://sites.google.com/site/monshonosuana/directx%E3%81%AE%E8%A9%B1/directx%E3%81%AE%E8%A9%B1-%E7%AC%AC176%E5%9B%9E)

	//!< barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = buffer.Get();

	//!< barrierの実行
	context.GetCommandList()->ResourceBarrier(1, &barrier);
}

void AccelerationStructure::Update(
	const GraphicsCommandContext& context,
	const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs) {

	//!< 更新フラグが設定されていない場合はエラー.
	STREAM_ASSERT(flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE, "acceleration structure does not allow updates.");
	STREAM_ASSERT(inputs.Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE, "acceleration structure update flag is not set.");

	//!< descの設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
	desc.Inputs                           = inputs;
	desc.SourceAccelerationStructureData  = buffer.GetGpuVirtualAddress();
	desc.DestAccelerationStructureData    = buffer.GetGpuVirtualAddress();
	desc.ScratchAccelerationStructureData = scratch.GetGpuVirtualAddress();

	//!< 実行
	context.GetCommandList()->BuildRaytracingAccelerationStructure(
		&desc,
		0, nullptr
	);

	//!< barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = buffer.Get();

	//!< barrierの実行
	context.GetCommandList()->ResourceBarrier(1, &barrier);
}

D3D12_GPU_VIRTUAL_ADDRESS AccelerationStructure::GetGpuVirtualAddress() const {
	STREAM_ASSERT(buffer != nullptr, "acceleration structure buffer is null.");
	return buffer.GetGpuVirtualAddress();
}

AccelerationStructure AccelerationStructure::Create(
	const Device& device,
	const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs) {

	AccelerationStructure acceleration;

	//!< 必要なメモリ数を求める
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
	device.GetDevice()->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

	acceleration.buffer  = AccelerationStructure::CreateAccelerationStructureBuffer(device, info);
	acceleration.scratch = AccelerationStructure::CreateScratchBuffer(device, info);

	acceleration.flags = inputs.Flags; //!< flagsの保存

	return acceleration;
}

Resource AccelerationStructure::CreateAccelerationStructureBuffer(
	const Device& device,
	const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO& info) {

	ResourceDesc desc = ResourceDesc::CreateBufferDesc(
		D3D12_HEAP_TYPE_DEFAULT,
		info.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE
	);

	Resource buffer = Resource::CreateCommitted(device, desc);
	buffer.SetName(L"Acceleration Structure | Acceleration Structure Buffer");

	return buffer;
}

Resource AccelerationStructure::CreateScratchBuffer(
	const Device& device,
	const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO& info) {

	ResourceDesc desc = ResourceDesc::CreateBufferDesc(
		D3D12_HEAP_TYPE_DEFAULT,
		std::max(info.ScratchDataSizeInBytes, info.UpdateScratchDataSizeInBytes), //!< update用のscratchのサイズも考慮する
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON
	);

	Resource buffer = Resource::CreateCommitted(device, desc);
	buffer.SetName(L"Acceleration Structure | Scratch Buffer");

	return buffer;
}
