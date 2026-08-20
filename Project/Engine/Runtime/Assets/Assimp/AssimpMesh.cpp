#include "AssimpMesh.h"
SXAVENGER_ENGINE_USING_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpMesh class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string_view AssimpMesh::GetName() const {
	const aiString& name = mesh_->mName;
	return std::string_view(name.data, name.length);
}

uint32_t AssimpMesh::GetVertexCount() const {
	return mesh_->mNumVertices;
}

bool AssimpMesh::HasPosition() const {
	return mesh_->HasPositions();
}

Vector3f AssimpMesh::GetPosition(uint32_t index) const {
	StreamLogger::Assert(mesh_->HasPositions(), "mesh has no positions.");
	StreamLogger::Assert(index < GetVertexCount(), "index is out of bounds.");
	const aiVector3D& position = mesh_->mVertices[index];
	return { position.x, position.y, position.z };
}

bool AssimpMesh::HasNormal() const {
	return mesh_->HasNormals();
}

Vector3f AssimpMesh::GetNormal(uint32_t index) const {
	StreamLogger::Assert(mesh_->HasNormals(), "mesh has no normals.");
	StreamLogger::Assert(index < GetVertexCount(), "index is out of bounds.");
	const aiVector3D& normal = mesh_->mNormals[index];
	return { normal.x, normal.y, normal.z };
}

bool AssimpMesh::HasTexcoord(uint32_t channel) const {
	return mesh_->HasTextureCoords(channel);
}

Vector2f AssimpMesh::GetTexcoord(uint32_t index, uint32_t channel) const {
	StreamLogger::Assert(mesh_->HasTextureCoords(channel), std::format("mesh has no texcoord. channel: {}", channel));
	StreamLogger::Assert(index < GetVertexCount(), "index is out of bounds.");
	const aiVector3D& texcoord = mesh_->mTextureCoords[channel][index];
	return { texcoord.x, texcoord.y };
}

bool AssimpMesh::HasTangentBitangent() const {
	return mesh_->HasTangentsAndBitangents();
}

Vector3f AssimpMesh::GetTangent(uint32_t index) const {
	StreamLogger::Assert(mesh_->HasTangentsAndBitangents(), "mesh has no tangents and bitangents.");
	StreamLogger::Assert(index < GetVertexCount(), "index is out of bounds.");
	const aiVector3D& tangent = mesh_->mTangents[index];
	return { tangent.x, tangent.y, tangent.z };
}

Vector3f AssimpMesh::GetBitangent(uint32_t index) const {
	StreamLogger::Assert(mesh_->HasTangentsAndBitangents(), "mesh has no tangents and bitangents.");
	StreamLogger::Assert(index < GetVertexCount(), "index is out of bounds.");
	const aiVector3D& bitangent = mesh_->mBitangents[index];
	return { bitangent.x, bitangent.y, bitangent.z };
}
uint32_t AssimpMesh::GetFaceCount() const {
	return mesh_->mNumFaces;
}

AssimpMesh::Triangle AssimpMesh::GetTriangle(uint32_t index) const {
	StreamLogger::Assert(index < GetFaceCount(), "index is out of bounds.");
	const aiFace& face = mesh_->mFaces[index];

	StreamLogger::Assert(face.mNumIndices == 3, "face is not triangle.");
	return Triangle{ face.mIndices[0], face.mIndices[1], face.mIndices[2] };
}

bool AssimpMesh::HasBone() const {
	return mesh_->mNumBones != 0;
}

uint32_t AssimpMesh::GetBoneCount() const {
	return mesh_->mNumBones;
}
