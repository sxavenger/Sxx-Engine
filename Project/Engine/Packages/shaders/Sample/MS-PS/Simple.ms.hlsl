//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Simple.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

ConstantBuffer<Component::Camera> gCameraProjection : register(b0);
ConstantBuffer<Component::Transform> gCameraTransform : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("mesh")]
[numthreads(MESHLET_MAX_TRIANGLE_COUNT, 1, 1)]
[outputtopology("triangle")]
void main(
	ComputeSemantics semantics,
	in payload MeshletPayload payload,
	out vertices PixelInputData vertices[MESHLET_MAX_VERTEX_COUNT],
	out indices uint32_t3 triangles[MESHLET_MAX_TRIANGLE_COUNT],
	out primitives MeshletPrimitive primitives[MESHLET_MAX_TRIANGLE_COUNT]) {

	uint32_t meshlet_index = payload.meshlet_indices[semantics.group_id.x];

	if (meshlet_index >= meshlet_count) {
		return;
	}

	Meshlet meshlet = gMeshlets[meshlet_index];
	meshlet.SetMeshOutputCount(); //!< meshの出力数を設定する

	//!< triangleの出力
	if (semantics.group_thread_id.x < meshlet.triangle_count) {

		uint32_t3 local_triangle = gTriangles[meshlet.triangle_offset + semantics.group_thread_id.x].GetTriangle();
		triangles[semantics.group_thread_id.x] = local_triangle;

		// TODO: primitive cullingの実装
		primitives[semantics.group_thread_id.x].cull = false;

	}

	//!< vertexの出力
	if (semantics.group_thread_id.x < meshlet.vertex_count) {

		uint32_t vertex_index   = gVertexIndices[meshlet.vertex_offset + semantics.group_thread_id.x];
		StaticMeshVertex vertex = gVertices[vertex_index];
		float32_t3 position     = gPositions[vertex_index];

		PixelInputData output = (PixelInputData)0;

		//!< 頂点情報の登録
		output.position  = position;
		output.normal    = vertex.GetNormal();
		output.tangent   = vertex.GetTangent();
		output.bitangent = vertex.GetBitangent();
		output.texcoord  = vertex.GetTexcoord();

		float32_t4x4 view       = gCameraTransform.GetView();
		float32_t4x4 projection = gCameraProjection.GetProjection();

		output.raster.position = mul(float32_t4(position, 1.0f), mul(view, projection));
		output.raster.meshlet_index = meshlet_index;

		vertices[semantics.group_thread_id.x] = output;

	}

}
