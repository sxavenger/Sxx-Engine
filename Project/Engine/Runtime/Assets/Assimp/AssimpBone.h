#pragma once

//-----------------------------------------------------------------------------------------
// include
//----------------------------------------------------------------------------------------
//* assets
#include "AssimpOption.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Pointer/ReferencePointer.h>
#include <Lib/Math/Transform/Transformation3d.h>

//* assimp
#include <assimp/mesh.h>

//* c++
#include <string_view>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// AssimpBone class
////////////////////////////////////////////////////////////////////////////////////////////
class AssimpBone final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// VertexWeight structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct VertexWeight {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		float weight;   //!< 頂点に対するジョイントの影響度(ウェイト)
		uint32_t index; //!< 影響を受ける頂点のindex

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	AssimpBone() = delete;
	AssimpBone(const aiBone* bone) noexcept : bone_(bone) {}

	//* bone option *//

	std::string_view GetName() const;

	uint32_t GetVertexWeightCount() const;

	VertexWeight GetVertexWeight(uint32_t index) const;

	Transformation3d GetOffsetTransformation() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	RefPtr<const aiBone> bone_ = nullptr;

};

SXAVENGER_ENGINE_NAMESPACE_END
