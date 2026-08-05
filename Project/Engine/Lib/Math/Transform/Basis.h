#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* math
#include "../Vector3.h"
#include "../Quaternion.h"

//* c++
#include <concepts>
#include <array>
#include <initializer_list>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// Basis structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Basis {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Basis() noexcept = default;

	//* operator [copy / move] <Basis> */

	constexpr Basis(const Basis&) noexcept            = default;
	constexpr Basis& operator=(const Basis&) noexcept = default;

	//* operator [copy] <std::initializer_list> *//

	constexpr Basis(const std::initializer_list<float>& list) noexcept;
	constexpr Basis(const std::initializer_list<std::initializer_list<float>>& list) noexcept;

	//* operator [compound assignment] <Basis> *//

	constexpr Basis& operator*=(const Basis& rhs) noexcept;

	//* operator [binary] <Basis> *//

	constexpr Basis operator*(const Basis& rhs) const noexcept;

	//* constant value methods *//

	constexpr static Basis Identity() noexcept;

	//* mathmatical methods *//

	constexpr static Basis Transpose(const Basis& b) noexcept;

	constexpr static Basis Inverse(const Basis& b) noexcept;

	static Basis MakeScale(float scale) noexcept;
	static Basis MakeScale(const Vector3<float>& scale) noexcept;

	static Basis MakeRotateX(float angle) noexcept;
	static Basis MakeRotateY(float angle) noexcept;
	static Basis MakeRotateZ(float angle) noexcept;

	static Basis MakeRotate(const Vector3<float>& euler) noexcept;
	static Basis MakeRotate(const Quaternion<float>& quaternion) noexcept;

	//=========================================================================================
	// public variables
	//=========================================================================================

	constexpr static inline size_t kRowCount    = 3;
	constexpr static inline size_t kColumnCount = 3;

	std::array<std::array<float, kColumnCount>, kRowCount> m = {};

};

////////////////////////////////////////////////////////////////////////////////////////////
// Basis structure inline methods
////////////////////////////////////////////////////////////////////////////////////////////

constexpr Basis::Basis(const std::initializer_list<float>& list) noexcept {
	for (size_t row = 0; row < kRowCount; ++row) {
		for (size_t col = 0; col < kColumnCount; ++col) {
			m[row][col] = *(list.begin() + row * kColumnCount + col);
		}
	}
}

constexpr Basis::Basis(const std::initializer_list<std::initializer_list<float>>& list) noexcept {
	for (size_t row = 0; row < kRowCount; ++row) {
		for (size_t col = 0; col < kColumnCount; ++col) {
			m[row][col] = *((list.begin() + row)->begin() + col);
		}
	}
}

constexpr Basis& Basis::operator*=(const Basis& rhs) noexcept {
	Basis mat = {};
	for (size_t row = 0; row < kRowCount; ++row) {
		for (size_t col = 0; col < kColumnCount; ++col) {
			for (size_t i = 0; i < kRowCount; ++i) {
				mat.m[row][col] += m[row][i] * rhs.m[i][col];
			}
		}
	}

	*this = mat;
	return *this;
}

constexpr Basis Basis::operator*(const Basis& rhs) const noexcept {
	Basis mat = {};
	for (size_t row = 0; row < kRowCount; ++row) {
		for (size_t col = 0; col < kColumnCount; ++col) {
			for (size_t i = 0; i < kRowCount; ++i) {
				mat.m[row][col] += m[row][i] * rhs.m[i][col];
			}
		}
	}

	return mat;
}

constexpr Basis Basis::Identity() noexcept {
	return {
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	};
}

constexpr Basis Basis::Transpose(const Basis& b) noexcept {
	Basis mat = {};
	for (size_t row = 0; row < kRowCount; ++row) {
		for (size_t col = 0; col < kColumnCount; ++col) {
			mat.m[row][col] = b.m[col][row];
		}
	}

	return mat;
}

constexpr Basis Basis::Inverse(const Basis& b) noexcept {
	Basis v = {
			b.m[1][1] * b.m[2][2] - b.m[1][2] * b.m[2][1],    // [0][0]
			-(b.m[0][1] * b.m[2][2] - b.m[0][2] * b.m[2][1]), // [0][1]
			b.m[0][1] * b.m[1][2] - b.m[0][2] * b.m[1][1],    // [0][2]

			-(b.m[1][0] * b.m[2][2] - b.m[1][2] * b.m[2][0]), // [1][0]
			b.m[0][0] * b.m[2][2] - b.m[0][2] * b.m[2][0],    // [1][1]
			-(b.m[0][0] * b.m[1][2] - b.m[0][2] * b.m[1][0]), // [1][2]

			b.m[1][0] * b.m[2][1] - b.m[1][1] * b.m[2][0],    // [2][0]
			-(b.m[0][0] * b.m[2][1] - b.m[0][1] * b.m[2][0]), // [2][1]
			b.m[0][0] * b.m[1][1] - b.m[0][1] * b.m[1][0],    // [2][2]
	};

	float denominator = (b.m[0][0] * b.m[1][1] * b.m[2][2])
		+ (b.m[0][1] * b.m[1][2] * b.m[2][0])
		+ (b.m[0][2] * b.m[1][0] * b.m[2][1])
		- (b.m[0][2] * b.m[1][1] * b.m[2][0])
		- (b.m[0][1] * b.m[1][0] * b.m[2][2])
		- (b.m[0][0] * b.m[1][2] * b.m[2][1]);

	if (denominator == 0.0) { //!< 0除算対策
		return {};
	}

	for (size_t row = 0; row < kRowCount; row++) {
		for (size_t column = 0; column < kColumnCount; column++) {
			v.m[row][column] /= denominator;
		}
	}

	return v;
}

inline Basis Basis::MakeScale(float scale) noexcept {
	Basis mat = Basis::Identity();
	mat.m[0][0] = scale;
	mat.m[1][1] = scale;
	mat.m[2][2] = scale;

	return mat;
}

inline Basis Basis::MakeScale(const Vector3<float>& scale) noexcept {
	Basis mat = Basis::Identity();
	mat.m[0][0] = scale.x;
	mat.m[1][1] = scale.y;
	mat.m[2][2] = scale.z;

	return mat;
}

inline Basis Basis::MakeRotateX(float angle) noexcept {
	Basis mat = Basis::Identity();
	mat.m[1][1] = std::cos(angle);
	mat.m[1][2] = -std::sin(angle);
	mat.m[2][1] = std::sin(angle);
	mat.m[2][2] = std::cos(angle);

	return mat;
}

inline Basis Basis::MakeRotateY(float angle) noexcept {
	Basis mat = Basis::Identity();
	mat.m[0][0] = std::cos(angle);
	mat.m[0][2] = std::sin(angle);
	mat.m[2][0] = -std::sin(angle);
	mat.m[2][2] = std::cos(angle);

	return mat;
}

inline Basis Basis::MakeRotateZ(float angle) noexcept {
	Basis mat = Basis::Identity();
	mat.m[0][0] = std::cos(angle);
	mat.m[0][1] = -std::sin(angle);
	mat.m[1][0] = std::sin(angle);
	mat.m[1][1] = std::cos(angle);

	return mat;
}

inline Basis Basis::MakeRotate(const Vector3<float>& euler) noexcept {
	return Basis::MakeRotateX(euler.x) * Basis::MakeRotateY(euler.y) * Basis::MakeRotateZ(euler.z);
}

inline Basis Basis::MakeRotate(const Quaternion<float>& quaternion) noexcept {
	Basis mat = Basis::Identity();
	mat.m[0][0] = 1 - 2 * (quaternion.imaginary.y * quaternion.imaginary.y + quaternion.imaginary.z * quaternion.imaginary.z);
	mat.m[0][1] = 2 * (quaternion.imaginary.x * quaternion.imaginary.y - quaternion.w * quaternion.imaginary.z);
	mat.m[0][2] = 2 * (quaternion.imaginary.x * quaternion.imaginary.z + quaternion.w * quaternion.imaginary.y);
	mat.m[1][0] = 2 * (quaternion.imaginary.x * quaternion.imaginary.y + quaternion.w * quaternion.imaginary.z);
	mat.m[1][1] = 1 - 2 * (quaternion.imaginary.x * quaternion.imaginary.x + quaternion.imaginary.z * quaternion.imaginary.z);
	mat.m[1][2] = 2 * (quaternion.imaginary.y * quaternion.imaginary.z - quaternion.w * quaternion.imaginary.x);
	mat.m[2][0] = 2 * (quaternion.imaginary.x * quaternion.imaginary.z - quaternion.w * quaternion.imaginary.y);
	mat.m[2][1] = 2 * (quaternion.imaginary.y * quaternion.imaginary.z + quaternion.w * quaternion.imaginary.x);
	mat.m[2][2] = 1 - 2 * (quaternion.imaginary.x * quaternion.imaginary.x + quaternion.imaginary.y * quaternion.imaginary.y);

	return mat;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Mathmatical namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Math {

	////////////////////////////////////////////////////////////////////////////////////////
	// Mathematical functions <Basis - Vector3>
	////////////////////////////////////////////////////////////////////////////////////////

	constexpr Vector3<float> Transform(const Vector3<float>& v, const Basis& b) noexcept {
		Vector3<float> vector = {};
		vector.x = v.x * b.m[0][0] + v.y * b.m[1][0] + v.z * b.m[2][0];
		vector.y = v.x * b.m[0][1] + v.y * b.m[1][1] + v.z * b.m[2][1];
		vector.z = v.x * b.m[0][2] + v.y * b.m[1][2] + v.z * b.m[2][2];

		return vector;
	}
}
