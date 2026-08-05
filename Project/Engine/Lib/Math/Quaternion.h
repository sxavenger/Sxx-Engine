#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* math
#include "MathConstants.h"
#include "Vector3.h"

//* c++
#include <concepts>
#include <format>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion structure
////////////////////////////////////////////////////////////////////////////////////////////
template <std::floating_point T>
struct Quaternion {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Imaginary structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//!< 四元数の虚部を表す構造体
	struct Imaginary {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		constexpr Imaginary() noexcept = default;
		constexpr Imaginary(T _x, T _y, T _z) noexcept : x(_x), y(_y), z(_z) {};

		//* operator [copy / move] <Imaginary> *//

		constexpr Imaginary(const Imaginary&) noexcept            = default;
		constexpr Imaginary& operator=(const Imaginary&) noexcept = default;

		constexpr Imaginary(Imaginary&&) noexcept            = default;
		constexpr Imaginary& operator=(Imaginary&&) noexcept = default;

		//* operator [compound assignment] <Imaginary> *//

		constexpr Imaginary& operator+=(const Imaginary& rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
		constexpr Imaginary& operator-=(const Imaginary& rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
		constexpr Imaginary& operator*=(const Imaginary& rhs) noexcept { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
		constexpr Imaginary& operator/=(const Imaginary& rhs) noexcept { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }

		//* operator [compound assignment] <T> *//

		constexpr Imaginary& operator*=(const T rhs) noexcept { x *= rhs; y *= rhs; z *= rhs; return *this; }
		constexpr Imaginary& operator/=(const T rhs) noexcept { x /= rhs; y /= rhs; z /= rhs; return *this; }

		//* operator [binary] <Imaginary> *//

		constexpr Imaginary operator+(const Imaginary& rhs) const noexcept { return { x + rhs.x, y + rhs.y, z + rhs.z }; }
		constexpr Imaginary operator-(const Imaginary& rhs) const noexcept { return { x - rhs.x, y - rhs.y, z - rhs.z }; }
		constexpr Imaginary operator*(const Imaginary& rhs) const noexcept { return { x * rhs.x, y * rhs.y, z * rhs.z }; }
		constexpr Imaginary operator/(const Imaginary& rhs) const noexcept { return { x / rhs.x, y / rhs.y, z / rhs.z }; }

		//* operator [binary] <T> *//

		constexpr Imaginary operator*(const T rhs) const noexcept { return { x * rhs, y * rhs, z * rhs }; }
		friend constexpr Imaginary operator*(const T lhs, const Imaginary& rhs) noexcept { return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z }; }

		constexpr Imaginary operator/(const T rhs) const noexcept { return { x / rhs, y / rhs, z / rhs }; }
		friend constexpr Imaginary operator/(const T lhs, const Imaginary& rhs) noexcept { return { lhs / rhs.x, lhs / rhs.y, lhs / rhs.z }; }

		//* operator [unary] *//

		constexpr Imaginary operator+() const noexcept { return *this; }
		constexpr Imaginary operator-() const noexcept { return { -x, -y, -z }; }

		//* mathmatical methods *//

		constexpr static T Dot(const Imaginary& lhs, const Imaginary& rhs) noexcept {
			return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
		}

		constexpr static Imaginary Cross(const Imaginary& lhs, const Imaginary& rhs) noexcept {
			return {
				lhs.y * rhs.z - lhs.z * rhs.y,
				lhs.z * rhs.x - lhs.x * rhs.z,
				lhs.x * rhs.y - lhs.y * rhs.x
			};
		}

		//=========================================================================================
		// public variables
		//=========================================================================================

		T x, y, z;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Quaternion() noexcept = default;
	constexpr Quaternion(T _x, T _y, T _z, T _w) noexcept : imaginary(_x, _y, _z), w(_w) {};
	constexpr Quaternion(const Imaginary& _imaginary, T _w) noexcept : imaginary(_imaginary), w(_w) {};

	//* operator [copy / move] <Quaternion> *//

	constexpr Quaternion(const Quaternion&) noexcept            = default;
	constexpr Quaternion& operator=(const Quaternion&) noexcept = default;

	constexpr Quaternion(Quaternion&&) noexcept            = default;
	constexpr Quaternion& operator=(Quaternion&&) noexcept = default;

	//* operator [compound assignment] <Quaternion> *//

	constexpr Quaternion& operator+=(const Quaternion& rhs) noexcept;
	constexpr Quaternion& operator-=(const Quaternion& rhs) noexcept;
	constexpr Quaternion& operator*=(const Quaternion& rhs) noexcept;

	//* operator [binary] <Quaternion> *//

	constexpr Quaternion operator+(const Quaternion& rhs) const noexcept;
	constexpr Quaternion operator-(const Quaternion& rhs) const noexcept;
	constexpr Quaternion operator*(const Quaternion& rhs) const noexcept;

	//* operator [binary] <T> *//

	constexpr Quaternion operator*(const T rhs) const noexcept { return { imaginary * rhs, w * rhs }; }
	friend constexpr Quaternion operator*(const T lhs, const Quaternion& rhs) noexcept { return { rhs.imaginary * lhs, rhs.w * lhs }; }

	//* operator [unary] *//

	constexpr Quaternion operator+() const noexcept { return *this; }
	constexpr Quaternion operator-() const noexcept { return { -imaginary, -w }; }

	//* constant value methods *//

	constexpr static Quaternion Identity() noexcept { return { 0, 0, 0, 1 }; }

	//* mathmatical methods *//

	constexpr static Quaternion Conjugate(const Quaternion& q) noexcept;

	constexpr static T Norm(const Quaternion& q) noexcept;

	constexpr static Quaternion Normalize(const Quaternion& q) noexcept;

	constexpr static Quaternion Inverse(const Quaternion& q) noexcept;

	constexpr static T Dot(const Quaternion& x, const Quaternion& y) noexcept;

	static Quaternion Slerp(const Quaternion& x, const Quaternion& y, T t) noexcept;

	static Quaternion AxisAngle(const Vector3<T>& axis, T angle) noexcept;

	static Vector3<T> RotateVector(const Vector3<T>& v, const Quaternion& q) noexcept;

	static Quaternion FromToRotation(const Vector3<T>& from, const Vector3<T>& to) noexcept;

	static Quaternion LookForward(const Vector3<T>& forward, const Vector3<T>& up = Vector3<T>::UnitY()) noexcept;

	//=========================================================================================
	// public variables
	//=========================================================================================

	Imaginary imaginary;
	T w;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion structure template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <std::floating_point T>
constexpr Quaternion<T>& Quaternion<T>::operator+=(const Quaternion& rhs) noexcept {
	imaginary += rhs.imaginary;
	w         += rhs.w;

	return *this;
}

template <std::floating_point T>
constexpr Quaternion<T>& Quaternion<T>::operator-=(const Quaternion& rhs) noexcept {
	imaginary -= rhs.imaginary;
	w         -= rhs.w;

	return *this;
}

template <std::floating_point T>
constexpr Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& rhs) noexcept {
	Imaginary cross = Imaginary::Cross(imaginary, rhs.imaginary);
	T dot           = Imaginary::Dot(imaginary, rhs.imaginary);

	imaginary = cross + rhs.imaginary * w + imaginary * rhs.w;
	w         = w * rhs.w - dot;

	return *this;
}

template<std::floating_point T>
inline constexpr Quaternion<T> Quaternion<T>::operator+(const Quaternion& rhs) const noexcept {
	return { imaginary + rhs.imaginary, w + rhs.w };
}

template<std::floating_point T>
inline constexpr Quaternion<T> Quaternion<T>::operator-(const Quaternion& rhs) const noexcept {
	return { imaginary - rhs.imaginary, w - rhs.w };
}

template<std::floating_point T>
inline constexpr Quaternion<T> Quaternion<T>::operator*(const Quaternion& rhs) const noexcept {
	Imaginary cross = Imaginary::Cross(imaginary, rhs.imaginary);
	T dot           = Imaginary::Dot(imaginary, rhs.imaginary);

	return { cross + rhs.imaginary * w + imaginary * rhs.w, w * rhs.w - dot };
}

template <std::floating_point T>
constexpr Quaternion<T> Quaternion<T>::Conjugate(const Quaternion& q) noexcept {
	return { -q.imaginary, q.w };
}

template <std::floating_point T>
constexpr T Quaternion<T>::Norm(const Quaternion& q) noexcept {
	return std::sqrt(q.imaginary.x * q.imaginary.x + q.imaginary.y * q.imaginary.y + q.imaginary.z * q.imaginary.z + q.w * q.w);
}

template <std::floating_point T>
constexpr Quaternion<T> Quaternion<T>::Normalize(const Quaternion& q) noexcept {
	T norm = Quaternion<T>::Norm(q);
	return { q.imaginary / norm, q.w / norm };
}

template <std::floating_point T>
constexpr Quaternion<T> Quaternion<T>::Inverse(const Quaternion& q) noexcept {
	Quaternion conj = Quaternion<T>::Conjugate(q);
	T normSquared = Imaginary::Dot(q.imaginary, q.imaginary) + q.w * q.w;

	return { conj.imaginary / normSquared, conj.w / normSquared };
}

template <std::floating_point T>
constexpr T Quaternion<T>::Dot(const Quaternion& x, const Quaternion& y) noexcept {
	return Imaginary::Dot(x.imaginary, y.imaginary) + x.w * y.w;
}

template <std::floating_point T>
Quaternion<T> Quaternion<T>::Slerp(const Quaternion& x, const Quaternion& y, T t) noexcept {
	T dot = Imaginary::Dot(x.imaginary, y.imaginary);

		// fromとtoが同じ方向の場合
	if (dot >= T(1.0) - Math::kEpsilon<T>) {
		return Quaternion<T>::Identity();
	}

	// fromとtoが逆方向の場合
	if (dot <= T(-1.0) + Math::kEpsilon<T>) {
		const Vector3<T> orthogonal = std::abs(x.imaginary.x) > T(1.0) - Math::kEpsilon<T> ? Vector3<T>::UnitY() : Vector3<T>::UnitX();
		return { Vector3<T>::Cross(x.imaginary, orthogonal).Normalize(), T(0) };
	}

	Vector3<T> axis = Vector3<T>::Cross(x.imaginary, y.imaginary).Normalize();
	T angle = std::acos(dot);

	return Quaternion<T>::AxisAngle(axis, angle);
}

template <std::floating_point T>
Quaternion<T> Quaternion<T>::AxisAngle(const Vector3<T>& axis, T angle) noexcept {
	Vector3<T> imaginary = axis * std::sin(angle / T(2.0));
	return { imaginary.x, imaginary.y, imaginary.z, std::cos(angle / T(2.0)) };
}

template <std::floating_point T>
inline Vector3<T> Quaternion<T>::RotateVector(const Vector3<T>& v, const Quaternion& q) noexcept {
	Quaternion vector = { v.x, v.y, v.z, T(0) };
	Quaternion result = q * vector * Quaternion::Conjugate(q);
	return { result.imaginary.x, result.imaginary.y, result.imaginary.z };
}

template <std::floating_point T>
Quaternion<T> Quaternion<T>::FromToRotation(const Vector3<T>& from, const Vector3<T>& to) noexcept {
	T dot = Math::Dot(from, to);

	// fromとtoが同じ方向の場合
	if (dot >= T(1.0) - Math::kEpsilon<T>) {
		return Quaternion<T>::Identity();
	}

	// fromとtoが逆方向の場合
	if (dot <= T(-1.0) + Math::kEpsilon<T>) {
		const Vector3<T> orthogonal = std::abs(from.x) > T(1.0) - Math::kEpsilon<T> ? Vector3<T>::UnitY() : Vector3<T>::UnitX();
		return { Math::Normalize(Math::Cross(from, orthogonal)), T(0) };
	}

	Vector3<T> axis = Math::Normalize(Math::Cross(from, to));
	T angle         = std::acos(dot);

	return Quaternion<T>::AxisAngle(axis, angle);
}

template <std::floating_point T>
Quaternion<T> Quaternion<T>::LookForward(const Vector3<T>& forward, const Vector3<T>& up) noexcept {
	Quaternion q = Quaternion::FromToRotation(Vector3<T>::Forward(), forward);
	Vector3<T> right = Math::Normalize(Math::Cross(up, forward));
	Vector3<T> newUp = Math::Normalize(Math::Cross(forward, right));
	Vector3<T> modified = Quaternion::RotateVector(Vector3<T>::Up(), q);
	return Quaternion::FromToRotation(modified, newUp) * q;
}

////////////////////////////////////////////////////////////////////////////////////////////
// std::formatter - Quaternion<T>
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct std::formatter<Quaternion<T>, char> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Quaternion<T>& v, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {}, {})", v.imaginary.x, v.imaginary.y, v.imaginary.z, v.w);
	}

};

template <typename T>
struct std::formatter<Quaternion<T>, wchar_t> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::wformat_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Quaternion<T>& v, std::wformat_context& ctx) const {
		return std::format_to(ctx.out(), L"({}, {}, {}, {})", v.imaginary.x, v.imaginary.y, v.imaginary.z, v.w);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion utility
////////////////////////////////////////////////////////////////////////////////////////////

//* floating-point types *//

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;

////////////////////////////////////////////////////////////////////////////////////////////
// Mathmatical namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Math {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mathematical functions <Vector3 - Quaternion>
	////////////////////////////////////////////////////////////////////////////////////////////

	template <std::floating_point T>
	Quaternion<T> ConvertQuaternion(const Vector3<T>& euler) noexcept {
		Vector3<T> c = {
			std::cos(T(0.5) * euler.x),
			std::cos(T(0.5) * euler.y),
			std::cos(T(0.5) * euler.z),
		};

		Vector3<T> s = {
			std::sin(T(0.5) * euler.x),
			std::sin(T(0.5) * euler.y),
			std::sin(T(0.5) * euler.z),
		};

		Quaternion<T> quaterion = {};
		quaterion.w           = c.x * c.y * c.z + s.x * s.y * s.z;
		quaterion.imaginary.x = s.x * c.y * c.z - c.x * s.y * s.z;
		quaterion.imaginary.y = c.x * s.y * c.z + s.x * c.y * s.z;
		quaterion.imaginary.z = c.x * c.y * s.z - s.x * s.y * c.z;

		return quaterion;
	}

	template <std::floating_point T>
	Vector3<T> ConvertEuler(const Quaternion<T>& q) noexcept {
		T pitch = T(0);
		T yaw   = T(0);
		T roll  = T(0);

		{ //!< pitch
			T y = T(2) * (q.imaginary.y * q.imaginary.z + q.w * q.imaginary.x);
			T x = q.w * q.w - q.imaginary.x * q.imaginary.x - q.imaginary.y * q.imaginary.y + q.imaginary.z * q.imaginary.z;

			if (y <= Math::kEpsilon<T> && x <= Math::kEpsilon<T>) {
				pitch = std::atan2(q.imaginary.x, q.w) * T(2);

			} else {
				pitch = std::atan2(y, x);
			}
		}

		{ //!< roll
			T y = T(2) * (q.imaginary.x * q.imaginary.y + q.w * q.imaginary.z);
			T x = q.w * q.w + q.imaginary.x * q.imaginary.x - q.imaginary.y * q.imaginary.y - q.imaginary.z * q.imaginary.z;

			if (y <= Math::kEpsilon<T> && x <= Math::kEpsilon<T>) {
				roll = T(0);

			} else {
				roll = std::atan2(y, x);
			}
		}

		{ //!< yaw
			T x = -T(2) * (q.imaginary.x * q.imaginary.z - q.w * q.imaginary.y);
			yaw = std::asin(std::clamp(x, -T(1), T(1)));
		}

		return { pitch, yaw, roll };
	}

}
