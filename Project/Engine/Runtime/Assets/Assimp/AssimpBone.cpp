#include "AssimpBone.h"
SXAVENGER_ENGINE_USING_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpBone class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string_view AssimpBone::GetName() const {
	const aiString& name = bone_->mName;
	return std::string_view(name.C_Str(), name.length);
}

uint32_t AssimpBone::GetVertexWeightCount() const {
	return bone_->mNumWeights;
}

AssimpBone::VertexWeight AssimpBone::GetVertexWeight(uint32_t index) const {
	STREAM_ASSERT(index < bone_->mNumWeights, "index out of range.");
	return { bone_->mWeights[index].mWeight, bone_->mWeights[index].mVertexId };
}

Transformation3d AssimpBone::GetOffsetTransformation() const {

	const aiMatrix4x4& t = bone_->mOffsetMatrix;

	//!< aiMatrix4x4 (列ベクトル規約) → Basis (行ベクトル規約) : 3x3 を転置する.
	Transformation3d transformation = {};
	transformation.basis = {
		t.a1, t.b1, t.c1,
		t.a2, t.b2, t.c2,
		t.a3, t.b3, t.c3,
	};
	transformation.translation = { t.a4, t.b4, t.c4 };

	return transformation;
}
