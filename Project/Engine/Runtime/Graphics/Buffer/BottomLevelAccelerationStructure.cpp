#include "BottomLevelAccelerationStructure.h"
SXAVENGER_ENGINE_USING_(Graphics)

////////////////////////////////////////////////////////////////////////////////////////////
// [BottomLevelAccelerationStructure] Geometry structure methods
////////////////////////////////////////////////////////////////////////////////////////////

D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS BottomLevelAccelerationStructure::Geometry::GetInputs(bool isUpdate) const {
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
	inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs       = 1;
	inputs.pGeometryDescs = &desc;
	inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	if (isUpdate) {
		inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
	}

	return inputs;
}

BottomLevelAccelerationStructure::Geometry BottomLevelAccelerationStructure::Geometry::CreateTriangleGeometry(
	D3D12_GPU_VIRTUAL_ADDRESS vertexBufferAddress, UINT64 vertexStride, UINT vertexCount,
	D3D12_GPU_VIRTUAL_ADDRESS indexBufferAddress, UINT indexCount) {

	D3D12_RAYTRACING_GEOMETRY_DESC desc = {};
	desc.Type                                 = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	desc.Triangles.VertexBuffer.StartAddress  = vertexBufferAddress;
	desc.Triangles.VertexBuffer.StrideInBytes = vertexStride;
	desc.Triangles.VertexCount                = vertexCount;
	desc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT; //!< 頂点フォーマットはfloat3を想定
	desc.Triangles.IndexBuffer                = indexBufferAddress;
	desc.Triangles.IndexCount                 = indexCount;
	desc.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT; //!< インデックスフォーマットはuint32を想定

	return desc;
}

////////////////////////////////////////////////////////////////////////////////////////////
// BottomLevelAccelerationStructure class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BottomLevelAccelerationStructure::Build(
	const Device& device, const GraphicsCommandContext& context,
	const Geometry& geometry) {

	//!< input情報の設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = geometry.GetInputs(false);

	accelerationStructure_ = AccelerationStructure::Create(device, inputs); //!< input情報からbufferの生成
	accelerationStructure_.Build(context, inputs); //!< buildの実行
}

void BottomLevelAccelerationStructure::Update(
	const GraphicsCommandContext& context,
	const Geometry& geometry) {

	//!< input情報の設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = geometry.GetInputs(true);

	accelerationStructure_.Update(context, inputs); //!< buildの実行
}
