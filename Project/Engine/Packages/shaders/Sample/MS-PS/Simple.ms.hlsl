//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Simple.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

ConstantBuffer<Component::Transform> gTransform : register(b2);

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
	in payload Meshlet::Payload payload,
	out vertices PixelInputData vertices[MESHLET_MAX_VERTEX_COUNT],
	out indices uint32_t3 triangles[MESHLET_MAX_TRIANGLE_COUNT],
	out primitives Meshlet::Primitive primitives[MESHLET_MAX_TRIANGLE_COUNT]) {

	uint32_t meshlet_index = payload.meshlet_indices[semantics.group_id.x];

	if (meshlet_index >= meshlet_count) {
		return;
	}

	Meshlet::Description meshlet = gMeshlets[meshlet_index];
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

		uint32_t vertex_index     = gVertexIndices[meshlet.vertex_offset + semantics.group_thread_id.x];
		StaticMeshVertex vertex   = gVertices[vertex_index];
		float32_t3 local_position = gPositions[vertex_index];
		float32_t3 world_position = gTransform.TransformPoint(local_position);

		PixelInputData output = (PixelInputData)0;

		//!< 頂点情報の登録
		output.position  = world_position;
		output.normal    = gTransform.TransformNormal(vertex.GetNormal());
		output.tangent   = gTransform.TransformNormal(vertex.GetTangent());
		output.bitangent = gTransform.TransformNormal(vertex.GetBitangent());
		output.texcoord  = vertex.GetTexcoord();

		float32_t4x4 view       = gCameraTransform.GetView();
		float32_t4x4 projection = gCameraProjection.GetProjection();

		output.raster.position = mul(float32_t4(world_position, 1.0f), mul(view, projection));
		output.raster.meshlet_index = meshlet_index;

		vertices[semantics.group_thread_id.x] = output;

	}

}
