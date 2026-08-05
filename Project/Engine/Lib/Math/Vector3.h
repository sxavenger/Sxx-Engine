#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>
#include <concepts>
#include <limits>
#include <algorithm>
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Vector3 structure
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct Vector3 {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Vector3() noexcept = default;
	constexpr Vector3(T _x, T _y, T _z) noexcept : x(_x), y(_y), z(_z) {};

	//* operator [copy / move] <Vector3> *//

	constexpr Vector3(const Vector3&) noexcept            = default;
	constexpr Vector3& operator=(const Vector3&) noexcept = default;

	constexpr Vector3(Vector3&&) noexcept            = default;
	constexpr Vector3& operator=(Vector3&&) noexcept = default;

	//* operator [compound assignment] <Vector3> *//

	constexpr Vector3& operator+=(const Vector3& rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	constexpr Vector3& operator-=(const Vector3& rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	constexpr Vector3& operator*=(const Vector3& rhs) noexcept { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
	constexpr Vector3& operator/=(const Vector3& rhs) noexcept { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }

	//* operator [compound assignment] <T> *//

	constexpr Vector3& operator*=(const T rhs) noexcept { x *= rhs; y *= rhs; z *= rhs; return *this; }
	constexpr Vector3& operator/=(const T rhs) noexcept { x /= rhs; y /= rhs; z /= rhs; return *this; }

	//* operator [binary] <Vector3> *//

	constexpr Vector3 operator+(const Vector3& rhs) const noexcept { return { x + rhs.x, y + rhs.y, z + rhs.z }; }
	constexpr Vector3 operator-(const Vector3& rhs) const noexcept { return { x - rhs.x, y - rhs.y, z - rhs.z }; }
	constexpr Vector3 operator*(const Vector3& rhs) const noexcept { return { x * rhs.x, y * rhs.y, z * rhs.z }; }
	constexpr Vector3 operator/(const Vector3& rhs) const noexcept { return { x / rhs.x, y / rhs.y, z / rhs.z }; }

	//* operator [binary] <T> *//

	constexpr Vector3 operator*(const T rhs) const noexcept { return { x * rhs, y * rhs, z * rhs }; }
	friend constexpr Vector3 operator*(const T lhs, const Vector3& rhs) noexcept { return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z }; }

	constexpr Vector3 operator/(const T rhs) const noexcept { return { x / rhs, y / rhs, z / rhs }; }
	friend constexpr Vector3 operator/(const T lhs, const Vector3& rhs) noexcept { return { lhs / rhs.x, lhs / rhs.y, lhs / rhs.z }; }

	//* operator [cast] <Vector3> *//

	template <typename U>
	explicit constexpr operator Vector3<U>() const noexcept { return { static_cast<U>(x), static_cast<U>(y), static_cast<U>(z) }; }

	//* operator [unary] *//

	constexpr Vector3 operator+() const noexcept { return *this; }
	constexpr Vector3 operator-() const noexcept { return { -x, -y, -z }; }

	//* operator [access] *//

	constexpr T& operator[](size_t index) noexcept { return data[index]; }
	constexpr const T& operator[](size_t index) const noexcept { return data[index]; }

	constexpr T* operator&() noexcept { return data.data(); }
	constexpr const T* operator&() const noexcept { return data.data(); }

	//* constant value methods *//

	constexpr static Vector3 Origin() noexcept { return { T(0), T(0), T(0) }; }

	constexpr static Vector3 Unit() noexcept { return { T(1), T(1), T(1) }; }

	constexpr static Vector3 UnitX() noexcept { return { T(1), T(0), T(0) }; }

	constexpr static Vector3 UnitY() noexcept { return { T(0), T(1), T(0) }; }

	constexpr static Vector3 UnitZ() noexcept { return { T(0), T(0), T(1) }; }

	constexpr static Vector3 Forward() noexcept { return Vector3::UnitZ(); }

	constexpr static Vector3 Up() noexcept { return Vector3::UnitY(); }

	constexpr static Vector3 Infinity() noexcept { return { std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity() }; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	union {
#pragma warning(push)
#pragma warning(disable:4201) // [C4201](https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4201)
		struct {
			T x, y, z;
		};

		std::array<T, 3> data;
#pragma warning(pop)
	};

};

////////////////////////////////////////////////////////////////////////////////////////////
// std::formatter - Vector3<T>
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct std::formatter<Vector3<T>, char> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Vector3<T>& v, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
	}
	
};

template <typename T>
struct std::formatter<Vector3<T>, wchar_t> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::wformat_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Vector3<T>& v, std::wformat_context& ctx) const {
		return std::format_to(ctx.out(), L"({}, {}, {})", v.x, v.y, v.z);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Vector3 utility
////////////////////////////////////////////////////////////////////////////////////////////

//* integral types *//

using Vector3i  = Vector3<std::int32_t>;
using Vector3ui = Vector3<std::uint32_t>;
using Vector3sz = Vector3<std::size_t>;

//* floating-point types *//

using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;

////////////////////////////////////////////////////////////////////////////////////////////
// Mathmatical namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Math {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Algorithm methods
	////////////////////////////////////////////////////////////////////////////////////////////

	template <typename T>
	Vector3<T> Clamp(const Vector3<T>& v, const Vector3<T>& min, const Vector3<T>& max) noexcept {
		return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y), std::clamp(v.z, min.z, max.z) };
	}	

	template <typename T>
	Vector3<T> Clamp(const Vector3<T>& v, T min, T max) noexcept {
		return { std::clamp(v.x, min, max), std::clamp(v.y, min, max), std::clamp(v.z, min, max) };
	}

	template <typename T>
	Vector3<T> Min(const Vector3<T>& v, const Vector3<T>& min) noexcept {
		return { std::min(v.x, min.x), std::min(v.y, min.y), std::min(v.z, min.z) };
	}

	template <typename T>
	Vector3<T> Min(const Vector3<T>& v, T min) noexcept {
		return { std::min(v.x, min), std::min(v.y, min), std::min(v.z, min) };
	}

	template <typename T>
	Vector3<T> Max(const Vector3<T>& v, const Vector3<T>& max) noexcept {
		return { std::max(v.x, max.x), std::max(v.y, max.y), std::max(v.z, max.z) };
	}

	template <typename T>
	Vector3<T> Max(const Vector3<T>& v, T max) noexcept {
		return { std::max(v.x, max), std::max(v.y, max), std::max(v.z, max) };
	}

	template <typename T>
	Vector3<T> Saturate(const Vector3<T>& v) noexcept {
		return { std::clamp(v.x, T(0.0), T(1.0)), std::clamp(v.y, T(0.0), T(1.0)), std::clamp(v.z, T(0.0), T(1.0)) };
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mathmatical methods
	////////////////////////////////////////////////////////////////////////////////////////////

	template <typename T> requires std::floating_point<T>
	Vector3<T> Abs(const Vector3<T>& v) noexcept {
		return { std::abs(v.x), std::abs(v.y), std::abs(v.z) };
	}

	template <typename T> requires std::floating_point<T>
	Vector3<T> Floor(const Vector3<T>& v) noexcept {
		return { std::floor(v.x), std::floor(v.y), std::floor(v.z) };
	}

	template <typename T> requires std::floating_point<T>
	T Length(const Vector3<T>& v) noexcept {
		return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	template <typename T> requires std::floating_point<T>
	T Distance(const Vector3<T>& lhs, const Vector3<T>& rhs) noexcept {
		return Length(lhs - rhs);
	}

	template <typename T> requires std::floating_point<T>
	Vector3<T> Normalize(const Vector3<T>& v) noexcept {
		T len = Length(v);
		return len != T(0.0) ? v / len : Vector3<T>();
	}

	template <typename T> requires std::floating_point<T>
	T Dot(const Vector3<T>& lhs, const Vector3<T>& rhs) noexcept {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	template <typename T> requires std::floating_point<T>
	Vector3<T> Cross(const Vector3<T>& lhs, const Vector3<T>& rhs) noexcept {
		return {
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		};
	}

	template <typename T> requires std::floating_point<T>
	Vector3<T> Lerp(const Vector3<T>& x, const Vector3<T>& y, T t) noexcept {
		return x * (T(1.0) - t) + y * t;
	}

	template <typename T> requires std::floating_point<T>
	Vector3<T> Reflect(const Vector3<T>& v, const Vector3<T>& n) noexcept {
		return v - T(2.0) * Dot(v, n) * n;
	}

}
