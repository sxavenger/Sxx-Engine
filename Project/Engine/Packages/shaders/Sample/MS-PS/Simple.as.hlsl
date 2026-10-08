//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Simple.hlsli"

//=========================================================================================
// group shared
//=========================================================================================

groupshared MeshletPayload payload;

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("amplification")]
[numthreads(MESHLET_APPLIFICATION_NUMTHREAD, 1, 1)]
void main(ComputeSemantics semantics) {
	//!< dispatch_thread_id.xはmeshlet_index

	bool visible = false;

	if (semantics.dispatch_thread_id.x < meshlet_count.x) {
		visible = true; //!< TODO: meshlet cullingの実装
	}

	if (visible) {
		uint32_t index = WavePrefixCountBits(visible);
		payload.meshlet_indices[index] = semantics.dispatch_thread_id.x;
	}

	uint32_t visible_count = WaveActiveCountBits(visible);

	DispatchMesh(visible_count, 1, 1, payload);
}
