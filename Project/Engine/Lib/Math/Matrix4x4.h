#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* math
#include "MathConstants.h"
#include "Vector3.h"
#include "Quaternion.h"

//* c++
#include <concepts>
#include <initializer_list>
#include <array>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// define option
////////////////////////////////////////////////////////////////////////////////////////////
#define USE_DIRECTX_MATH //!< DirectXMathを使用する場合は有効化
//!< (Invrese<float>の高速化の計算のために使用する)

#ifdef USE_DIRECTX_MATH
#include <DirectXMath.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix4x4 structure
////////////////////////////////////////////////////////////////////////////////////////////
template <std::floating_point T>
struct Matrix4x4 {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Matrix4x4() noexcept = default;

	//* operator [copy / move] <Matrix4x4> *//

	constexpr Matrix4x4(const Matrix4x4&) noexcept            = default;
	constexpr Matrix4x4& operator=(const Matrix4x4&) noexcept = default;

	constexpr Matrix4x4(Matrix4x4&&) noexcept            = default;
	constexpr Matrix4x4& operator=(Matrix4x4&&) noexcept = default;

	//* operator [copy] <std::initializer_list> *//

	constexpr Matrix4x4(const std::initializer_list<T>& list) noexcept;
	constexpr Matrix4x4(const std::initializer_list<std::initializer_list<T>>& list) noexcept;

	//* operator [compound assignment] <Matrix4x4> *//

	constexpr Matrix4x4& operator*=(const Matrix4x4& rhs) noexcept;

	//* operator [binary] <Matrix4x4> *//

	constexpr Matrix4x4 operator*(const Matrix4x4& rhs) const noexcept;

	//* constant value methods *//

	constexpr static Matrix4x4 Identity() noexcept;

	//* mathmatical methods *//

	constexpr static Matrix4x4 Transpose(const Matrix4x4& m) noexcept;

	static Matrix4x4 Inverse(const Matrix4x4& m) noexcept;

	static Matrix4x4 MakeScale(T scale) noexcept;
	static Matrix4x4 MakeScale(const Vector3<T>& scale) noexcept;

	static Matrix4x4 MakeRotateX(T radian) noexcept;
	static Matrix4x4 MakeRotateY(T radian) noexcept;
	static Matrix4x4 MakeRotateZ(T radian) noexcept;

	static Matrix4x4 MakeRotate(const Vector3<T>& euler) noexcept;
	static Matrix4x4 MakeRotate(const Quaternion<T>& quaternion) noexcept;

	static Matrix4x4 MakeTranslate(const Vector3<T>& translation) noexcept;

	static Matrix4x4 MakeAffine(const Vector3<T>& translation, const Vector3<T>& euler, const Vector3<T>& scale) noexcept;
	static Matrix4x4 MakeAffine(const Vector3<T>& translation, const Quaternion<T>& rotation, const Vector3<T>& scale) noexcept;

	static Matrix4x4 MakePerspectiveFov(T fov, T aspect, T nearZ, T farZ) noexcept;

	static Matrix4x4 MakeOrthographic(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept;

	static Matrix4x4 MakeViewport(T x, T y, T width, T height, T nearZ, T farZ) noexcept;

	//=========================================================================================
	// public variables
	//=========================================================================================

	constexpr static inline size_t kRowCount    = 4;
	constexpr static inline size_t kColumnCount = 4;

	std::array<std::array<T, kColumnCount>, kRowCount> m = {};

};

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix4x4 structure tempalate methods
////////////////////////////////////////////////////////////////////////////////////////////

template <std::floating_point T>
constexpr Matrix4x4<T>::Matrix4x4(const std::initializer_list<T>& list) noexcept {
	for (size_t row = 0; row < kRowCount; ++row) {
		for (size_t col = 0; col < kColumnCount; ++col) {
			m[row][col] = *(list.begin() + row * kColumnCount + col);
		}
	}
}

template <std::floating_point T>
constexpr Matrix4x4<T>::Matrix4x4(const std::initializer_list<std::initializer_list<T>>& list) noexcept {
	for (size_t row = 0; row < kRowCount; ++row) {
		for (size_t col = 0; col < kColumnCount; ++col) {
			m[row][col] = *((list.begin() + row)->begin() + col);
		}
	}
}

template <std::floating_point T>
constexpr Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4& rhs) noexcept {
	Matrix4x4 mat = {};
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

template <std::floating_point T>
constexpr Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4& rhs) const noexcept {
	Matrix4x4 mat = {};
	for (size_t row = 0; row < kRowCount; ++row) {
		for (size_t col = 0; col < kColumnCount; ++col) {
			for (size_t i = 0; i < kRowCount; ++i) {
				mat.m[row][col] += m[row][i] * rhs.m[i][col];
			}
		}
	}

	return mat;
}

template <std::floating_point T>
constexpr Matrix4x4<T> Matrix4x4<T>::Identity() noexcept {
	return {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
}

template <std::floating_point T>
constexpr Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4& m) noexcept {
	Matrix4x4 mat = {};
	for (size_t row = 0; row < kRowCount; ++row) {
		for (size_t col = 0; col < kColumnCount; ++col) {
			mat.m[row][col] = m.m[col][row];
		}
	}

	return mat;
}

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::Inverse(const Matrix4x4& m) noexcept {

	Matrix4x4<T> mat = {};

#ifdef USE_DIRECTX_MATH
	if constexpr (std::same_as<T, float>) {
		DirectX::XMMATRIX xm = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(m.m.data())));
		DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(mat.m.data()), xm);
		return mat;
	} else
#endif
	{
		mat.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3]) + (m.m[1][2] * m.m[2][3] * m.m[3][1]) + (m.m[1][3] * m.m[2][1] * m.m[3][2])
			- (m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[1][1] * m.m[2][3] * m.m[3][2]);
		mat.m[0][1] = -(m.m[0][1] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[2][1] * m.m[3][2])
			+ (m.m[0][3] * m.m[2][2] * m.m[3][1]) + (m.m[0][2] * m.m[2][1] * m.m[3][3]) + (m.m[0][1] * m.m[2][3] * m.m[3][2]);
		mat.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3]) + (m.m[0][2] * m.m[1][3] * m.m[3][1]) + (m.m[0][3] * m.m[1][1] * m.m[3][2])
			- (m.m[0][3] * m.m[1][2] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[3][2]);
		mat.m[0][3] = -(m.m[0][1] * m.m[1][2] * m.m[2][3]) - (m.m[0][2] * m.m[1][3] * m.m[2][1]) - (m.m[0][3] * m.m[1][1] * m.m[2][2])
			+ (m.m[0][3] * m.m[1][2] * m.m[2][1]) + (m.m[0][2] * m.m[1][1] * m.m[2][3]) + (m.m[0][1] * m.m[1][3] * m.m[2][2]);

		mat.m[1][0] = -(m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[1][3] * m.m[2][0] * m.m[3][2])
			+ (m.m[1][3] * m.m[2][2] * m.m[3][0]) + (m.m[1][2] * m.m[2][0] * m.m[3][3]) + (m.m[1][0] * m.m[2][3] * m.m[3][2]);
		mat.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3]) + (m.m[0][2] * m.m[2][3] * m.m[3][0]) + (m.m[0][3] * m.m[2][0] * m.m[3][2])
			- (m.m[0][3] * m.m[2][2] * m.m[3][0]) - (m.m[0][2] * m.m[2][0] * m.m[3][3]) - (m.m[0][0] * m.m[2][3] * m.m[3][2]);
		mat.m[1][2] = -(m.m[0][0] * m.m[1][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][3] * m.m[3][0]) - (m.m[0][3] * m.m[1][0] * m.m[3][2])
			+ (m.m[0][3] * m.m[1][2] * m.m[3][0]) + (m.m[0][2] * m.m[1][0] * m.m[3][3]) + (m.m[0][0] * m.m[1][3] * m.m[3][2]);
		mat.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3]) + (m.m[0][2] * m.m[1][3] * m.m[2][0]) + (m.m[0][3] * m.m[1][0] * m.m[2][2])
			- (m.m[0][3] * m.m[1][2] * m.m[2][0]) - (m.m[0][2] * m.m[1][0] * m.m[2][3]) - (m.m[0][0] * m.m[1][3] * m.m[2][2]);

		mat.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3]) + (m.m[1][1] * m.m[2][3] * m.m[3][0]) + (m.m[1][3] * m.m[2][0] * m.m[3][1])
			- (m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[1][0] * m.m[2][3] * m.m[3][1]);
		mat.m[2][1] = -(m.m[0][0] * m.m[2][1] * m.m[3][3]) - (m.m[0][1] * m.m[2][3] * m.m[3][0]) - (m.m[0][3] * m.m[2][0] * m.m[3][1])
			+ (m.m[0][3] * m.m[2][1] * m.m[3][0]) + (m.m[0][1] * m.m[2][0] * m.m[3][3]) + (m.m[0][0] * m.m[2][3] * m.m[3][1]);
		mat.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3]) + (m.m[0][1] * m.m[1][3] * m.m[3][0]) + (m.m[0][3] * m.m[1][0] * m.m[3][1])
			- (m.m[0][3] * m.m[1][1] * m.m[3][0]) - (m.m[0][1] * m.m[1][0] * m.m[3][3]) - (m.m[0][0] * m.m[1][3] * m.m[3][1]);
		mat.m[2][3] = -(m.m[0][0] * m.m[1][1] * m.m[2][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0]) - (m.m[0][3] * m.m[1][0] * m.m[2][1])
			+ (m.m[0][3] * m.m[1][1] * m.m[2][0]) + (m.m[0][1] * m.m[1][0] * m.m[2][3]) + (m.m[0][0] * m.m[1][3] * m.m[2][1]);

		mat.m[3][0] = -(m.m[1][0] * m.m[2][1] * m.m[3][2]) - (m.m[1][1] * m.m[2][2] * m.m[3][0]) - (m.m[1][2] * m.m[2][0] * m.m[3][1])
			+ (m.m[1][2] * m.m[2][1] * m.m[3][0]) + (m.m[1][1] * m.m[2][0] * m.m[3][2]) + (m.m[1][0] * m.m[2][2] * m.m[3][1]);
		mat.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2]) + (m.m[0][1] * m.m[2][2] * m.m[3][0]) + (m.m[0][2] * m.m[2][0] * m.m[3][1])
			- (m.m[0][2] * m.m[2][1] * m.m[3][0]) - (m.m[0][1] * m.m[2][0] * m.m[3][2]) - (m.m[0][0] * m.m[2][2] * m.m[3][1]);
		mat.m[3][2] = -(m.m[0][0] * m.m[1][1] * m.m[3][2]) - (m.m[0][1] * m.m[1][2] * m.m[3][0]) - (m.m[0][2] * m.m[1][0] * m.m[3][1])
			+ (m.m[0][2] * m.m[1][1] * m.m[3][0]) + (m.m[0][1] * m.m[1][0] * m.m[3][2]) + (m.m[0][0] * m.m[1][2] * m.m[3][1]);
		mat.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2]) + (m.m[0][1] * m.m[1][2] * m.m[2][0]) + (m.m[0][2] * m.m[1][0] * m.m[2][1])
			- (m.m[0][2] * m.m[1][1] * m.m[2][0]) - (m.m[0][1] * m.m[1][0] * m.m[2][2]) - (m.m[0][0] * m.m[1][2] * m.m[2][1]);

		T denominator = (m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]) + (m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]) + (m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2])
			- (m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2])
			- (m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2])
			+ (m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]) + (m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]) + (m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2])
			+ (m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]) + (m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]) + (m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2])
			- (m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2])
			- (m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0])
			+ (m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]) + (m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]) + (m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0]);

		if (denominator == 0) { //!< 0除算対策
			return {};
		};

		for (size_t row = 0; row < kRowCount; row++) {
			for (size_t column = 0; column < kColumnCount; column++) {
				mat.m[row][column] /= denominator;
			}
		}

		return mat;
	}
}

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::MakeScale(T scale) noexcept {
	Matrix4x4<T> mat = Matrix4x4<T>::Identity();
	mat.m[0][0] = scale;
	mat.m[1][1] = scale;
	mat.m[2][2] = scale;

	return mat;
}

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::MakeScale(const Vector3<T>& scale) noexcept {
	Matrix4x4<T> mat = Matrix4x4<T>::Identity();
	mat.m[0][0] = scale.x;
	mat.m[1][1] = scale.y;
	mat.m[2][2] = scale.z;

	return mat;
}

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::MakeRotateX(T radian) noexcept {
	Matrix4x4<T> mat = Matrix4x4<T>::Identity();
	mat.m[1][1] = std::cos(radian);
	mat.m[1][2] = -std::sin(radian);
	mat.m[2][1] = std::sin(radian);
	mat.m[2][2] = std::cos(radian);

	return mat;
}

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::MakeRotateY(T radian) noexcept {
	Matrix4x4<T> mat = Matrix4x4<T>::Identity();
	mat.m[0][0] = std::cos(radian);
	mat.m[0][2] = std::sin(radian);
	mat.m[2][0] = -std::sin(radian);
	mat.m[2][2] = std::cos(radian);

	return mat;
}

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::MakeRotateZ(T radian) noexcept {
	Matrix4x4<T> mat = Matrix4x4<T>::Identity();
	mat.m[0][0] = std::cos(radian);
	mat.m[0][1] = -std::sin(radian);
	mat.m[1][0] = std::sin(radian);
	mat.m[1][1] = std::cos(radian);

	return mat;
}

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::MakeRotate(const Vector3<T>& euler) noexcept {
	return Matrix4x4<T>::MakeRotateX(euler.x) * Matrix4x4<T>::MakeRotateY(euler.y) * Matrix4x4<T>::MakeRotateZ(euler.z);
}

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::MakeRotate(const Quaternion<T>& quaternion) noexcept {
	Matrix4x4<T> mat = Matrix4x4<T>::Identity();
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

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::MakeTranslate(const Vector3<T>& translation) noexcept {
	Matrix4x4<T> mat = Matrix4x4<T>::Identity();
	mat.m[0][3] = translation.x;
	mat.m[1][3] = translation.y;
	mat.m[2][3] = translation.z;

	return mat;
}

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::MakeAffine(const Vector3<T>& translation, const Vector3<T>& rotation, const Vector3<T>& scale) noexcept {
	return Matrix4x4<T>::MakeTranslate(translation) * Matrix4x4<T>::MakeRotate(rotation) * Matrix4x4<T>::MakeScale(scale);
}

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::MakeAffine(const Vector3<T>& translation, const Quaternion<T>& rotation, const Vector3<T>& scale) noexcept {
	return Matrix4x4<T>::MakeTranslate(translation) * Matrix4x4<T>::MakeRotate(rotation) * Matrix4x4<T>::MakeScale(scale);
}

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::MakePerspectiveFov(T fov, T aspect, T nearZ, T farZ) noexcept {
	return {
		1.0 / (aspect * std::tan(fov * 0.5)), 0.0, 0.0, 0.0,
		0.0, 1.0 / std::tan(fov * 0.5), 0.0, 0.0,
		0.0, 0.0, farZ / (farZ - nearZ), 1.0,
		0.0, 0.0, -nearZ * farZ / (farZ - nearZ), 0.0
	};
}

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::MakeOrthographic(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept {
	return {
		2.0 / (right - left), 0.0, 0.0, 0.0,
		0.0, 2.0 / (top - bottom), 0.0, 0.0,
		0.0, 0.0, 1.0 / (farZ - nearZ), 0.0,
		(left + right) / (left - right), (top + bottom) / (bottom - top), nearZ / (nearZ - farZ), 1.0
	};
}

template <std::floating_point T>
Matrix4x4<T> Matrix4x4<T>::MakeViewport(T x, T y, T width, T height, T nearZ, T farZ) noexcept {
	return {
		width / 2.0, 0.0, 0.0, 0.0,
		0.0, -height / 2.0, 0.0, 0.0,
		0.0, 0.0, farZ - nearZ, 0.0,
		x + (width / 2.0), y + (height / 2.0), nearZ, 1.0,
	};
}

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix4x4 utility
////////////////////////////////////////////////////////////////////////////////////////////

//* floating-point types *//

using Matrix4x4f = Matrix4x4<float>;
using Matrix4x4d = Matrix4x4<double>;

////////////////////////////////////////////////////////////////////////////////////////////
// Mathmatical namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Math {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mathematical functions <Matrix4x4 - Quaternion, Vector3>
	////////////////////////////////////////////////////////////////////////////////////////////

	template <std::floating_point T>
	Vector3<T> Transform(const Vector3<T>& v, const Matrix4x4<T>& m) noexcept {
		Vector3<T> vector = {};
		vector.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + m.m[0][3];
		vector.y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + m.m[1][3];
		vector.z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + m.m[2][3];

		float w = v.x * m.m[3][0] + v.y * m.m[3][1] + v.z * m.m[3][2] + m.m[3][3];

		if (w == 0.0) {
			return {}; //!< 0除算対策
		}

		return vector / w;
	}

	template <std::floating_point T>
	Vector3<T> TransformNormal(const Vector3<T>& v, const Matrix4x4<T>& m) noexcept {
		Vector3<T> vector = {};
		vector.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2];
		vector.y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2];
		vector.z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2];

		return vector;
	}

} // namespace Math
