#include "SlateTypes.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Math/VectorComparison.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Geometry structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Slate::Geometry Slate::Geometry::MakeChild(const Vector2f& offset, const Vector2f& size) const {
	return Geometry{ absolutePosition + offset, size, scale };
}

bool Slate::Geometry::ContainsAbsolute(const Vector2f& p) const {
	return Comparison::All(p >= absolutePosition) && Comparison::All(p < absolutePosition + localSize);
}
