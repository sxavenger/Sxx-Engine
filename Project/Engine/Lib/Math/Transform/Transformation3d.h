#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* transform
#include "Basis.h"

//* math
#include "../Vector3.h"
#include "../Matrix4x4.h"

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Transformation3d structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Transformation3d {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Transformation3d() noexcept : basis(Basis::Identity()), translation(Vector3f::Origin()) {};
	constexpr Transformation3d(const Basis& _basis, const Vector3f& _translation) noexcept : basis(_basis), translation(_translation) {};

	//* operator [copy / move] <Transformation3d> *//

	constexpr Transformation3d(const Transformation3d&) noexcept = default;
	constexpr Transformation3d& operator=(const Transformation3d&) noexcept = default;

	//* operator [compound assignment] <Transformation3d> *//

	constexpr Transformation3d& operator*=(const Transformation3d& rhs) noexcept;

	//* operator [binary] <Transformation3d> *//

	constexpr Transformation3d operator*(const Transformation3d& rhs) const noexcept;

	//* constant value methods *//

	constexpr static Transformation3d Identity() noexcept;

	//* mathmatical methods *//

	constexpr static Transformation3d Inverse(const Transformation3d& t) noexcept;

	static Transformation3d MakeAffine(const Vector3f& scale, const Vector3f& rotation, const Vector3f& translation) noexcept;
	static Transformation3d MakeAffine(const Vector3f& scale, const Quaternionf& rotation, const Vector3f& translation) noexcept;

	constexpr static Matrix4x4f ConvertMatrix(const Transformation3d& t) noexcept;

	//=========================================================================================
	// public variables
	//=========================================================================================

	Basis basis          = {};
	Vector3f translation = {};

};

////////////////////////////////////////////////////////////////////////////////////////////
// Transformation3d structure inline methods
////////////////////////////////////////////////////////////////////////////////////////////

constexpr Transformation3d& Transformation3d::operator*=(const Transformation3d& rhs) noexcept {
	basis       *= rhs.basis;
	translation = Math::Transform(translation, rhs.basis) + rhs.translation;

	return *this;
}

constexpr Transformation3d Transformation3d::operator*(const Transformation3d& rhs) const noexcept {
	Transformation3d transformation = {};
	transformation.basis       = basis * rhs.basis;
	transformation.translation = Math::Transform(translation, rhs.basis) + rhs.translation;

	return transformation;
}

constexpr Transformation3d Transformation3d::Identity() noexcept {
	return { Basis::Identity(), Vector3f::Origin() };
}

constexpr Transformation3d Transformation3d::Inverse(const Transformation3d& t) noexcept {
	Transformation3d transformation = {};
	transformation.basis       = Basis::Inverse(t.basis);
	transformation.translation = Math::Transform(-t.translation, transformation.basis);

	return transformation;
}

inline Transformation3d Transformation3d::MakeAffine(const Vector3f& scale, const Vector3f& rotation, const Vector3f& translation) noexcept {
	Transformation3d transformation = {};
	transformation.basis       = Basis::MakeScale(scale) * Basis::MakeRotate(rotation);
	transformation.translation = translation;

	return transformation;
}

inline Transformation3d Transformation3d::MakeAffine(const Vector3f& scale, const Quaternionf& rotation, const Vector3f& translation) noexcept {
	Transformation3d transformation = {};
	transformation.basis       = Basis::MakeScale(scale) * Basis::MakeRotate(rotation);
	transformation.translation = translation;

	return transformation;
}

constexpr Matrix4x4f Transformation3d::ConvertMatrix(const Transformation3d& t) noexcept {
	return {
		t.basis.m[0][0], t.basis.m[0][1], t.basis.m[0][2], 0.0f,
		t.basis.m[1][0], t.basis.m[1][1], t.basis.m[1][2], 0.0f,
		t.basis.m[2][0], t.basis.m[2][1], t.basis.m[2][2], 0.0f,
		t.translation.x, t.translation.y, t.translation.z, 1.0f
	};
}

////////////////////////////////////////////////////////////////////////////////////////////
// Mathmatical namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Math {

	////////////////////////////////////////////////////////////////////////////////////////
	// Mathematical functions <Basis - Vector3>
	////////////////////////////////////////////////////////////////////////////////////////

	constexpr Vector3f Transform(const Vector3f& v, const Transformation3d& t) noexcept {
		return Math::Transform(v, t.basis) + t.translation;
	}
}
