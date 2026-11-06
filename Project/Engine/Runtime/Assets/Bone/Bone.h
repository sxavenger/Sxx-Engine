#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "../Base/BaseAsset.h"
#include "BoneMetadata.h"

//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Transform/Transformation3d.h>
#include <Lib/Math/Transform/Transform3d.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Assets)

////////////////////////////////////////////////////////////////////////////////////////////
// Bone class
////////////////////////////////////////////////////////////////////////////////////////////
class Bone final
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

		Transform3d GetTransform() const;

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::string name;
		Transformation3d transformation;
		std::vector<Description> children;
		std::vector<Uuid> meshIds;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	Bone(const BaseAssetMetadata& metadata) noexcept : BaseAsset(metadata) {}
	~Bone() noexcept override = default;

	//* description option *//

	const Description& GetDescription() const;

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* directory *//

	static inline const std::filesystem::path kDirectory = L"Bone"; //!< meshの保存ディレクトリ

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* metadata *//

	BoneMetadata metadata_;

	//* description *//

	Description description_;

	//-----------------------------------------------------------------------------------------
	// friend class
	//-----------------------------------------------------------------------------------------
	friend class BoneBuilder;

};

SXAVENGER_ENGINE_NAMESPACE_END
