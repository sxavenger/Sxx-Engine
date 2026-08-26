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
	x = PackedTexcoord::Encode(v.x);
	y = PackedTexcoord::Encode(v.y);
}

PackedTexcoord& PackedTexcoord::operator=(const Vector2f& v) noexcept {
	x = PackedTexcoord::Encode(v.x);
	y = PackedTexcoord::Encode(v.y);

	return *this;
}

PackedTexcoord::operator Vector2f() const noexcept {
	return Vector2f{ PackedTexcoord::Decode(x), PackedTexcoord::Decode(y) };
}

uint16_t PackedTexcoord::Encode(float value) noexcept {
	constexpr const float kScale = static_cast<float>(std::numeric_limits<uint16_t>::max());
	return static_cast<uint16_t>(std::lround(std::clamp(value, 0.0f, 1.0f) * kScale));
}

float PackedTexcoord::Decode(uint16_t value) noexcept {
	constexpr const float kScale = 1.0f / static_cast<float>(std::numeric_limits<uint16_t>::max());
	return static_cast<float>(value) * kScale;
}
