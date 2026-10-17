#include "TopLevelAccelerationStructure.h"
SXAVENGER_ENGINE_USING_(Graphics)


//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// TopLevelAccelerationStructure class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TopLevelAccelerationStructure::ResetInstance() {
	instances_.clear();
}

void TopLevelAccelerationStructure::AddInstance(const Instance& instance) {
	instances_.emplace_back(instance);
}

void TopLevelAccelerationStructure::SetupInstanceDesc(const Device& device, const GraphicsCommandContext& context, ResourceAllocator& allocator) {

	BuildMode mode = PrepareInstanceDesc(device, allocator);

	switch (mode) {
		case BuildMode::Build:
			Build(device, context, allocator);
			break;

		case BuildMode::Update:
			Update(device, context);
			break;
	}
}

TopLevelAccelerationStructure::BuildMode TopLevelAccelerationStructure::PrepareInstanceDesc(const Device& device, ResourceAllocator& allocator) {

	BuildMode mode = BuildMode::Update;

	if (!descs_.HasHandle() || instances_.size() > descs_.GetCapacity()) {
		//!< descが未作成または, capacityが足りない場合は再作成
		uint32_t capacity = std::max<uint32_t>(1, static_cast<uint32_t>(instances_.size())); //!< capacityは1以上にする必要がある.
		descs_ = DimensionBuffer<D3D12_RAYTRACING_INSTANCE_DESC>::Create(device, allocator, capacity, Graphics::kFrameCount);
		mode = BuildMode::Build;
	}

	// previousInstanceCountとの比較
	if (previousInstanceCount_ != instances_.size()) {
		previousInstanceCount_ = instances_.size();
		mode = BuildMode::Build;
	}

	descs_.Fill(D3D12_RAYTRACING_INSTANCE_DESC{});

	for (const auto& [i, instance] : instances_ | std::views::enumerate) {

		//!< descの設定
		D3D12_RAYTRACING_INSTANCE_DESC desc = {};
		desc.AccelerationStructure               = instance.blas->GetGpuVirtualAddress();
		desc.InstanceMask                        = instance.instanceMask.to_ulong();
		desc.InstanceID                          = instance.instanceId;
		desc.Flags                               = instance.flags.value;
		desc.InstanceContributionToHitGroupIndex = static_cast<UINT>(i); // HACK: 完全に同じobjectの場合, この値は同じになる

		//!< matrixを3x4に変更
		Matrix4x4f mat = Matrix4x4f::Transpose(instance.transform);
		std::memcpy(desc.Transform, &mat, sizeof(desc.Transform));

		descs_.At(i) = desc;
	}

	return mode;
}

void TopLevelAccelerationStructure::Build(const Device& device, const GraphicsCommandContext& context, ResourceAllocator& allocator) {

	//!< input情報の設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
	inputs.Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout   = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs      = static_cast<UINT>(instances_.size());
	inputs.InstanceDescs = descs_.GetGpuVirtualAddress();
	inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	accelerationStructure_ = AccelerationStructure::Create(device, allocator, inputs); //!< input情報からbufferの生成
	accelerationStructure_.Build(context, inputs); //!< buildの実行
}

void TopLevelAccelerationStructure::Update(const Device& device, const GraphicsCommandContext& context) {

	// input情報の設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
	inputs.Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout   = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs      = static_cast<UINT>(instances_.size());
	inputs.InstanceDescs = descs_.GetGpuVirtualAddress();
	inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	accelerationStructure_.Update(context, inputs); //!< updateの実行
}
