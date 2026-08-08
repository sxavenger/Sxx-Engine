#include "SlateTypes.h"
SXAVENGER_ENGINE_USING_(Editor)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Math/VectorComparison.h>

//* c++
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// Geometry structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Slate::Geometry Slate::Geometry::MakeChild(const Vector2f& offset, const Vector2f& size) const {
	return Geometry{ absolutePosition + offset, size, scale };
}

bool Slate::Geometry::ContainsAbsolute(const Vector2f& p) const {
	return Comparison::All(p >= absolutePosition) && Comparison::All(p < absolutePosition + localSize);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Utility methods
////////////////////////////////////////////////////////////////////////////////////////////

Color4f Slate::ConvertToLinearColor(const Color4f& color) {

	//!< sRGB EOTF. (IEC 61966-2-1)
	const auto convert = [](float channel) {
		if (channel <= 0.04045f) {
			return channel / 12.92f;
		}

		return std::pow((channel + 0.055f) / 1.055f, 2.4f);
	};

	return Color4f{ convert(color.r), convert(color.g), convert(color.b), color.a }; //!< alphaは変換しない.
}
