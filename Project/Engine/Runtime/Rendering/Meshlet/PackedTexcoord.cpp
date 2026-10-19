#include "PackedTexcoord.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <algorithm>
#include <limits>

////////////////////////////////////////////////////////////////////////////////////////////
// PackedTexcoord structure methods
////////////////////////////////////////////////////////////////////////////////////////////

PackedTexcoord::PackedTexcoord(const Vector2f& v) noexcept {
	x = v.x;
	y = v.y;
}

PackedTexcoord& PackedTexcoord::operator=(const Vector2f& v) noexcept {
	x = v.x;
	y = v.y;

	return *this;
}

PackedTexcoord::operator Vector2f() const noexcept {
	return Vector2f{ x, y };
}
