//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Simple.hlsli"

//* meshlet
#include "../Meshlet/StaticMeshVertex.hlsli"

//* component
#include "../Component/Camera.hlsli"
#include "../Component/Transform.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

StructuredBuffer<float3> gPositions : register(t0);
StructuredBuffer<StaticMeshVertex> gVertices : register(t1);

ConstantBuffer<Component::Camera> gCameraProjection : register(b0);
ConstantBuffer<Component::Transform> gCameraTransform : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
FragmentInputData main(uint32_t vertex_id : SV_VertexID) {

	FragmentInputData output;

	float3 position         = gPositions[vertex_id];
	StaticMeshVertex vertex = gVertices[vertex_id];

	//!< 頂点情報の登録
	output.position  = position;
	output.normal    = vertex.GetNormal();
	output.tangent   = vertex.GetTangent();
	output.bitangent = vertex.GetBitangent();
	output.texcoord  = vertex.GetTexcoord();

	float32_t4x4 view       = gCameraTransform.GetView();
	float32_t4x4 projection = gCameraProjection.GetProjection();

	output.raster.position = mul(float32_t4(position, 1.0f), mul(view, projection));

	return output;

}
