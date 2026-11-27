#include "MeshVertexBuffer.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Graphics/Core.h>

////////////////////////////////////////////////////////////////////////////////////////////
// [MeshVertexBuffer] Vertex structure methods
////////////////////////////////////////////////////////////////////////////////////////////

PackedNormal MeshVertexBuffer::Vertex::EncodeTangent(const Vector3f& normal, const Vector3f& tangent, const Vector3f& bitangent) {
	//!< tangentの方向を正規化する
	float bias = (Math::Dot(Math::Cross(tangent, bitangent), normal) < 0.0f) ? -1.0f : 1.0f;
	return Vector4f{ tangent.x, tangent.y, tangent.z, bias };
}

////////////////////////////////////////////////////////////////////////////////////////////
// MeshVertexBuffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

MeshVertexBuffer MeshVertexBuffer::Create(uint32_t capacity, uint8_t frameCount) {
	return MeshVertexBuffer{ Graphics::Core::CreateDimensionBuffer<MeshVertexBuffer::Vertex>(capacity, frameCount) };
}
