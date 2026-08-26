#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Runtime/Foundation.hpp>

//* lib
#include <Lib/Math/Vector3.h>
#include <Lib/Math/Vector4.h>

//* c++
#include <cstdint>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN_(Rendering)

////////////////////////////////////////////////////////////////////////////////////////////
// PackedNormal structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PackedNormal final {
	// 参考文献
	// [UE6 - FPackedNormal.h](https://github.com/EpicGames/UnrealEngine/blob/ue6-main/Engine/Source/Runtime/RenderCore/Public/PackedNormal.h)
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	PackedNormal() noexcept : packed(0) {}

	//* operator [copy / move] <PackedNormal> *//

	PackedNormal(const PackedNormal&) noexcept            = default;
	PackedNormal& operator=(const PackedNormal&) noexcept = default;

	PackedNormal(PackedNormal&&) noexcept            = default;
	PackedNormal& operator=(PackedNormal&&) noexcept = default;

	//* operator [comparison] <PackedNormal> *//

	bool operator==(const PackedNormal& rhs) const noexcept { return packed == rhs.packed; }
	bool operator!=(const PackedNormal& rhs) const noexcept { return packed != rhs.packed; }

	//* operator [assignment] <Vector3f> *//

	PackedNormal(const Vector3f& v) noexcept;
	PackedNormal& operator=(const Vector3f& v) noexcept;

	//* operator [cast] <Vector3f> *//

	operator Vector3f() const noexcept;
	Vector3f ToVector3f() const noexcept { return static_cast<Vector3f>(*this); }

	//* operator [assignment] <Vector4f> *//

	PackedNormal(const Vector4f& v) noexcept;
	PackedNormal& operator=(const Vector4f& v) noexcept;

	//* operator [cast] <Vector4f> *//

	operator Vector4f() const noexcept;
	Vector4f ToVector4f() const noexcept { return static_cast<Vector4f>(*this); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	union {
#pragma warning(push)
#pragma warning(disable:4201) // [C4201](https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4201)
		struct {
			int8_t x, y, z, w;
		};

		std::array<int8_t, 4> data;

		uint32_t packed;
#pragma warning(pop)
	};

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//! @brief float値をint8_tに変換(エンコード)する.
	static int8_t Encode(float value) noexcept;

	//! @brief int8_t値をfloatに変換(デコード)する.
	static float Decode(int8_t value) noexcept;

};

SXAVENGER_ENGINE_NAMESPACE_END
