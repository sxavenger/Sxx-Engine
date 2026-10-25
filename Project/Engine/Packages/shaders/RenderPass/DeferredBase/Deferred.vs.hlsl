//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Deferred.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("vertex")]
PixelInputData main(Semantics::Vertex semantics) {

	//!< 頂点情報の取得
	float32_t3 local_position = gPositions[semantics.vertex_id];
	StaticMeshVertex vertex   = gVertices[semantics.vertex_id];

	PixelInputData output = BuildPixelInputData(local_position, vertex);

	return output;

}
