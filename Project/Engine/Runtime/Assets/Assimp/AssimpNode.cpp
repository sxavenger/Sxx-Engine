#include "AssimpNode.h"
SXAVENGER_ENGINE_USING_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpNode class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string_view AssimpNode::GetName() const {
	return node_->mName.C_Str();
}

uint32_t AssimpNode::GetChildCount() const {
	return node_->mNumChildren;
}

AssimpNode AssimpNode::GetChild(uint32_t index) const {
	return node_->mChildren[index];
}

uint32_t AssimpNode::GetMeshCount() const {
	return node_->mNumMeshes;
}

uint32_t AssimpNode::GetMeshIndex(uint32_t index) const {
	return node_->mMeshes[index];
}

Transform3d AssimpNode::GetTransform() const {
	const aiMatrix4x4& t = node_->mTransformation;

	//!< aiMatrix4x4 (列ベクトル規約) → Basis (行ベクトル規約) : 3x3 を転置する.
	Transformation3d transformation = {};
	transformation.basis = {
		t.a1, t.b1, t.c1,
		t.a2, t.b2, t.c2, 
		t.a3, t.b3, t.c3,
	};
	transformation.translation = { t.a4, t.b4, t.c4 };

	//!< Transform3dに変換する.
	return Transform3d::Decompose(transformation);
}

