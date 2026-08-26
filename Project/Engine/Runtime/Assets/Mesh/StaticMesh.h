#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/BaseAsset.h"
#include "Mesh.h"
#include "StaticMeshMetadata.h"

//* engine
#include <Runtime/Foundation.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// StaticMesh class
////////////////////////////////////////////////////////////////////////////////////////////
class StaticMesh final
	: public BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Description structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Description {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		std::vector<uint32_t> GetIndices() const;

		void SetIndices(const std::vector<uint32_t>& indices);

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::vector<MeshVertex> vertices;
		std::vector<MeshPolygon> polygons;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	StaticMesh(const BaseAssetMetadata& metadata) noexcept : BaseAsset(metadata) {}
	~StaticMesh() noexcept override = default;

	//* description option *//

	const Description& GetDescription() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* metadata *//

	StaticMeshMetadata metadata_;

	//* description *//

	Description description_;

	//-----------------------------------------------------------------------------------------
	// friend class
	//-----------------------------------------------------------------------------------------
	friend class StaticMeshBuilder;

};

SXAVENGER_ENGINE_NAMESPACE_END
