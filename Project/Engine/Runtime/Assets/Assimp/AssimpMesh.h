#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assimp
#include "AssimpOption.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Math/Vector2.h>
#include <Lib/Math/Vector3.h>
#include <Lib/Math/Transform/Transformation3d.h>

//* assimp
#include <assimp/mesh.h>

//* c++
#include <string_view>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpMesh class
////////////////////////////////////////////////////////////////////////////////////////////
class AssimpMesh final {
public:

	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------

	using Triangle = std::array<uint32_t, 3>; //!< 三角形の頂点インデックスを格納する配列

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	AssimpMesh() = delete;
	AssimpMesh(const aiMesh* mesh) noexcept : mesh_(mesh) {}

	//* mesh option *//

	std::string_view GetName() const;

	//* vertex option *//

	uint32_t GetVertexCount() const;

	bool HasPosition() const;

	Vector3f GetPosition(uint32_t index) const;

	bool HasNormal() const;

	Vector3f GetNormal(uint32_t index) const;

	bool HasTexcoord(uint32_t channel = 0) const;

	Vector2f GetTexcoord(uint32_t index, uint32_t channel = 0) const;

	bool HasTangentBitangent() const;

	Vector3f GetTangent(uint32_t index) const;

	Vector3f GetBitangent(uint32_t index) const;

	//* face option *//

	uint32_t GetFaceCount() const;

	Triangle GetTriangle(uint32_t index) const;

	//* bone option *//

	bool HasBone() const;

	uint32_t GetBoneCount() const;

	// TODO: Bone関係の関数の追加.

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	RefPtr<const aiMesh> mesh_ = nullptr;

};

SXAVENGER_ENGINE_NAMESPACE_END
