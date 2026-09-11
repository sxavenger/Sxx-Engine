//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Simple.hlsli"

//* meshlet
#include "../Meshlet/StaticMeshVertex.hlsli"

//* component
#include "../Component/Camera.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

StructuredBuffer<float3> gPositions : register(t0);
StructuredBuffer<StaticMeshVertex> gVertices : register(t1);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float32_t4x4 MakePerspective() {

	const float32_t2 sensor = float32_t2(16.0, 9.0);
	const float32_t focal   = 20.0;
	const float32_t near    = 0.1;
	const float32_t far     = 1024.0;

	float32_t aspect = sensor.x / sensor.y;
	float32_t fov    = 2.0 * atan((sensor.y * 0.5) / focal);

	return Component::Camera::MakePerspective(fov, aspect, near, far);
}

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

	float32_t4x4 view = float32_t4x4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, -1.0, 10.0, 1.0
	);
	float32_t4x4 projection = MakePerspective();
	//!< (0, 1, -10) の位置にカメラがあると仮定.

	output.raster.position = mul(float32_t4(position, 1.0f), mul(view, projection));

	return output;

}
