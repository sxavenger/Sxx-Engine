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

	static Transform3d Decompose(const Transformation3d& t) noexcept;
	static Transform3d Decompose(const Matrix4x4f& m) noexcept;

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
	return Matrix4x4f::MakeAffine(t.scale, t.rotation, t.translation);
}

inline Transform3d Transform3d::Decompose(const Transformation3d& t) noexcept {

	Transform3d result = Transform3d::Identity();

	//* translation
	result.translation = t.translation;

	//* scale
	// basis = S * R なので, 行 i の長さが scale[i] になる.
	const auto& b = t.basis.m;

	float scale[3] = {};
	for (size_t i = 0; i < 3; ++i) {
		scale[i] = std::sqrt(b[i][0] * b[i][0] + b[i][1] * b[i][1] + b[i][2] * b[i][2]);
	}

	// 行列式が負 (反射を含む) なら x軸の scale を反転して吸収する.
	const float determinant
		= b[0][0] * (b[1][1] * b[2][2] - b[1][2] * b[2][1])
		- b[0][1] * (b[1][0] * b[2][2] - b[1][2] * b[2][0])
		+ b[0][2] * (b[1][0] * b[2][1] - b[1][1] * b[2][0]);

	if (determinant < 0.0f) {
		scale[0] = -scale[0];
	}

	result.scale = { scale[0], scale[1], scale[2] };

	//* rotation
	// scale が 0 の軸があると回転を復元できない.
	for (size_t i = 0; i < 3; ++i) {
		if (std::abs(scale[i]) <= Math::kEpsilonf) {
			return result; //!< rotation = Identity
		}
	}

	// 各行を scale で割って純粋な回転行列にする.
	float r[3][3] = {};
	for (size_t i = 0; i < 3; ++i) {
		for (size_t j = 0; j < 3; ++j) {
			r[i][j] = b[i][j] / scale[i];
		}
	}

	// Shepperd法. Basis::MakeRotate(Quaternion) の要素配置の逆算.
	//   r[2][1] - r[1][2] = 4wx, r[0][2] - r[2][0] = 4wy, r[1][0] - r[0][1] = 4wz
	Quaternionf q = Quaternionf::Identity();
	const float trace = r[0][0] + r[1][1] + r[2][2];

	if (trace > 0.0f) {
		const float s = std::sqrt(trace + 1.0f) * 2.0f; //!< s = 4w
		q.w           = 0.25f * s;
		q.imaginary.x = (r[2][1] - r[1][2]) / s;
		q.imaginary.y = (r[0][2] - r[2][0]) / s;
		q.imaginary.z = (r[1][0] - r[0][1]) / s;

	} else if (r[0][0] > r[1][1] && r[0][0] > r[2][2]) {
		const float s = std::sqrt(1.0f + r[0][0] - r[1][1] - r[2][2]) * 2.0f; //!< s = 4x
		q.w           = (r[2][1] - r[1][2]) / s;
		q.imaginary.x = 0.25f * s;
		q.imaginary.y = (r[0][1] + r[1][0]) / s;
		q.imaginary.z = (r[0][2] + r[2][0]) / s;

	} else if (r[1][1] > r[2][2]) {
		const float s = std::sqrt(1.0f + r[1][1] - r[0][0] - r[2][2]) * 2.0f; //!< s = 4y
		q.w           = (r[0][2] - r[2][0]) / s;
		q.imaginary.x = (r[0][1] + r[1][0]) / s;
		q.imaginary.y = 0.25f * s;
		q.imaginary.z = (r[1][2] + r[2][1]) / s;

	} else {
		const float s = std::sqrt(1.0f + r[2][2] - r[0][0] - r[1][1]) * 2.0f; //!< s = 4z
		q.w           = (r[1][0] - r[0][1]) / s;
		q.imaginary.x = (r[0][2] + r[2][0]) / s;
		q.imaginary.y = (r[1][2] + r[2][1]) / s;
		q.imaginary.z = 0.25f * s;
	}

	result.rotation = Quaternionf::Normalize(q);
	return result;
}

inline Transform3d Transform3d::Decompose(const Matrix4x4f& m) noexcept {
	Transformation3d t = Transformation3d::FromMatrix(m);
	return Transform3d::Decompose(t);
}
