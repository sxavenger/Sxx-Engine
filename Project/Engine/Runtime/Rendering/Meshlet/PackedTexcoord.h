#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Vector2.h>

//* c++
#include <cstdint>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// PackedTexcoord structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PackedTexcoord final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	PackedTexcoord() noexcept : packed(0) {}

	//* operator [copy / move] <PackedTexcoord> *//

	PackedTexcoord(const PackedTexcoord&) noexcept            = default;
	PackedTexcoord& operator=(const PackedTexcoord&) noexcept = default;

	PackedTexcoord(PackedTexcoord&&) noexcept            = default;
	PackedTexcoord& operator=(PackedTexcoord&&) noexcept = default;

	//* operator [comparison] <PackedTexcoord> *//

	bool operator==(const PackedTexcoord& rhs) const noexcept { return packed == rhs.packed; }
	bool operator!=(const PackedTexcoord& rhs) const noexcept { return packed != rhs.packed; }

	//* operator [assignment] <Vector2f> *//

	PackedTexcoord(const Vector2f& v) noexcept;
	PackedTexcoord& operator=(const Vector2f& v) noexcept;

	//* operator [cast] <Vector2f> *//

	operator Vector2f() const noexcept;
	Vector2f ToVector2f() const noexcept { return static_cast<Vector2f>(*this); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	union {
#pragma warning(push)
#pragma warning(disable:4201) // [C4201](https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4201)
		struct {
			uint16_t x, y;
		};

		std::array<uint16_t, 2> data;

		uint32_t packed;
#pragma warning(pop)
	};

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//! @brief float値をuint16_tに変換(エンコード)する.
	static uint16_t Encode(float value) noexcept;

	//! @brief uint16_t値をfloatに変換(デコード)する.
	static float Decode(uint16_t value) noexcept;

};

SXAVENGER_ENGINE_NAMESPACE_END
