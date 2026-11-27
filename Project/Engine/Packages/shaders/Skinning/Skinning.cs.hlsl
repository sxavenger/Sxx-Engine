//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* system
#include "../System/Semantics.hlsli"

//* meshlet
#include "../Meshlet/MeshVertex.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//! @brief コンテキスト情報
cbuffer Context : register(b0) {
	uint32_t vertex_count;
	uint32_t joint_count;
}

//! @brief 頂点情報
StructuredBuffer<float32_t3> gPositions : register(t0);
StructuredBuffer<MeshVertex> gVertices  : register(t1);

//! @brief 出力頂点情報
RWStructuredBuffer<float32_t3> gOutPositions : register(u0);
RWStructuredBuffer<MeshVertex> gOutVertices  : register(u1);

//! @brief 影響頂点情報
struct Influence {

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint32_t4 joint_indices; //!< 影響頂点のインデックス
	float32_t4 weights;      //!< 影響度

	static const uint32_t kCount = 4; //!< 影響頂点数

};
StructuredBuffer<Influence> gInfluences : register(t2);

//! @brief パレット変形行列
struct PaletteTransform {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float32_t4x4 world;
	float32_t4x4 world_inverse;

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* transform option *//

	float32_t4 TransformPoint(float32_t4 position) {
		return mul(position, world);
	}

	float32_t3 TransformPoint(float32_t3 position) {
		return mul(float32_t4(position, 1.0f), world).xyz;
	}

	float32_t3 TransformNormal(float32_t3 normal) {
		return mul(normal, (float32_t3x3)transpose(world_inverse));
		//!< 法線の変換のため, world_inverse の転置行列を使用する.
	}

};
StructuredBuffer<PaletteTransform> gPalette : register(t3);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("compute")]
[numthreads(32, 1, 1)] //!< vertex_countでdispatchする.
void main(Semantics::Compute semantics) {

	uint32_t vertex_index = semantics.dispatch_thread_id.x;

	if (vertex_index >= vertex_count) {
		return; //!< 頂点数を超えた場合は処理しない
	}

	float32_t3 position = gPositions[vertex_index];
	MeshVertex vertex   = gVertices[vertex_index];

	float32_t3 normal  = vertex.normal.Unpack3();
	float32_t4 tangent = vertex.tangent.Unpack4();

	Influence influence = gInfluences[vertex_index];

	if (all(influence.weights == float32_t4(0.0, 0.0, 0.0, 0.0))) {
		//!< 影響頂点がない場合は, 変形せずにそのまま出力する.
		gOutPositions[vertex_index] = position;
		gOutVertices[vertex_index]  = vertex;
		return;
	}

	float32_t3 skinned_position = float32_t3(0.0, 0.0, 0.0);
	float32_t3 skinned_normal   = float32_t3(0.0, 0.0, 0.0);
	float32_t3 skinned_tangent  = float32_t3(0.0, 0.0, 0.0);

	float32_t weight_sum = 0.0;

	[unroll]
	for (uint32_t i = 0; i < Influence::kCount; ++i) {

		float32_t weight = influence.weights[i];
		uint32_t index   = influence.joint_indices[i];

		if (index >= joint_count) {
			continue; //!< joint_countを超えた場合は処理しない
		}

		PaletteTransform transform = gPalette[index];

		skinned_position += transform.TransformPoint(position) * weight;
		skinned_normal   += transform.TransformNormal(normal) * weight;
		skinned_tangent  += transform.TransformNormal(tangent.xyz) * weight;

		weight_sum += weight;
	}

	skinned_position /= weight_sum; //!< 影響度の合計で正規化する.
	skinned_normal   = normalize(skinned_normal);
	skinned_tangent  = normalize(skinned_tangent);

	MeshVertex skinned_vertex = vertex;
	skinned_vertex.normal  = PackedNormal::Pack3(skinned_normal);
	skinned_vertex.tangent = PackedNormal::Pack4(float32_t4(skinned_tangent, (tangent.w < 0.0) ? -1.0 : 1.0)); //!< handnessを保持する.

	gOutPositions[vertex_index] = skinned_position;
	gOutVertices[vertex_index]  = skinned_vertex;
}
