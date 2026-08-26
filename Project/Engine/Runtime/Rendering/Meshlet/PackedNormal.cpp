#include "PackedNormal.h"
SXAVENGER_ENGINE_USING_(Rendering)

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <algorithm>
#include <limits>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// PackedNormal structure methods
////////////////////////////////////////////////////////////////////////////////////////////

PackedNormal::PackedNormal(const Vector3f& v) noexcept {
	x = PackedNormal::Encode(v.x);
	y = PackedNormal::Encode(v.y);
	z = PackedNormal::Encode(v.z);
	w = std::numeric_limits<int8_t>::max();
}

PackedNormal& PackedNormal::operator=(const Vector3f& v) noexcept {
	x = PackedNormal::Encode(v.x);
	y = PackedNormal::Encode(v.y);
	z = PackedNormal::Encode(v.z);
	w = std::numeric_limits<int8_t>::max();

	return *this;
}

PackedNormal::operator Vector3f() const noexcept {
	return Math::Normalize(Vector3f{ PackedNormal::Decode(x), PackedNormal::Decode(y), PackedNormal::Decode(z) });
}

PackedNormal::PackedNormal(const Vector4f& v) noexcept {
	x = PackedNormal::Encode(v.x);
	y = PackedNormal::Encode(v.y);
	z = PackedNormal::Encode(v.z);
	w = PackedNormal::Encode(v.w);
}

PackedNormal& PackedNormal::operator=(const Vector4f& v) noexcept {
	x = PackedNormal::Encode(v.x);
	y = PackedNormal::Encode(v.y);
	z = PackedNormal::Encode(v.z);
	w = PackedNormal::Encode(v.w);

	return *this;
}

PackedNormal::operator Vector4f() const noexcept {
	return Vector4f{ PackedNormal::Decode(x), PackedNormal::Decode(y), PackedNormal::Decode(z), PackedNormal::Decode(w) };
}

int8_t PackedNormal::Encode(float value) noexcept {
	constexpr const float kScale = static_cast<float>(std::numeric_limits<int8_t>::max());
	return static_cast<int8_t>(std::clamp<int32_t>(std::lround(value * kScale), std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max()));
}

float PackedNormal::Decode(int8_t value) noexcept {
	constexpr const float kScale = 1.0f / static_cast<float>(std::numeric_limits<int8_t>::max());
	return static_cast<float>(value) * kScale;
}
