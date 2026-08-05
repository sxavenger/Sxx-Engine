#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* transform
#include "Transformation3d.h"

//* math
#include "../Vector3.h"
#include "../Quaternion.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Transform3d structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Transform3d {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Transform3d() noexcept = default;
	constexpr Transform3d(const Vector3f& _scale, const Quaternionf& _rotation, const Vector3f& _translation) noexcept
		: scale(_scale), rotation(_rotation), translation(_translation) {
	};

	//* operator [copy / move] <Transform3d> *//

	constexpr Transform3d(const Transform3d&) noexcept            = default;
	constexpr Transform3d& operator=(const Transform3d&) noexcept = default;

	constexpr Transform3d(Transform3d&&) noexcept            = default;
	constexpr Transform3d& operator=(Transform3d&&) noexcept = default;

	//* constant value methods *//

	constexpr static Transform3d Identity() noexcept;

	//* mathmatical methods *//

	static Vector3f GetForward(const Transform3d& t) noexcept;

	static Transform3d MakeAffine(const Vector3f& scale, const Quaternionf& rotation, const Vector3f& translation) noexcept;
	static Transform3d MakeAffine(const Vector3f& scale, const Vector3f& rotation, const Vector3f& translation) noexcept;

	static Transformation3d ConvertTransformation(const Transform3d& t) noexcept;
	static Matrix4x4f ConvertMatrix(const Transform3d& t) noexcept;

	//=========================================================================================
	// public variables
	//=========================================================================================

	Vector3f scale       = Vector3f::Unit();
	Quaternionf rotation = Quaternionf::Identity();
	Vector3f translation = Vector3f::Origin();

};

////////////////////////////////////////////////////////////////////////////////////////////
// Transform3d structure inline methods
////////////////////////////////////////////////////////////////////////////////////////////

constexpr Transform3d Transform3d::Identity() noexcept {
	return { Vector3f::Unit(), Quaternionf::Identity(), Vector3f::Origin() };
}

inline Vector3f Transform3d::GetForward(const Transform3d& t) noexcept {
	return Quaternionf::RotateVector(Vector3f::Forward(), t.rotation);
}

inline Transform3d Transform3d::MakeAffine(const Vector3f& scale, const Quaternionf& rotation, const Vector3f& translation) noexcept {
	return { scale, rotation, translation };
}

inline Transform3d Transform3d::MakeAffine(const Vector3f& scale, const Vector3f& rotation, const Vector3f& translation) noexcept {
	return { scale, Math::ConvertQuaternion(rotation), translation };
}

inline Transformation3d Transform3d::ConvertTransformation(const Transform3d& t) noexcept {
	return Transformation3d::MakeAffine(t.scale, t.rotation, t.translation);
}

inline Matrix4x4f Transform3d::ConvertMatrix(const Transform3d& t) noexcept {
	return Matrix4x4f::MakeAffine(t.translation, t.rotation, t.scale);
}
