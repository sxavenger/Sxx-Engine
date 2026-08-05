#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>
#include <array>
#include <concepts>
#include <limits>
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Vector4 structure
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct Vector4 {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Vector4() noexcept = default;
	constexpr Vector4(T _x, T _y, T _z, T _w) noexcept : x(_x), y(_y), z(_z), w(_w) {};

	//* operator [copy / move] <Vector4> *//

	constexpr Vector4(const Vector4&) noexcept            = default;
	constexpr Vector4& operator=(const Vector4&) noexcept = default;

	constexpr Vector4(Vector4&&) noexcept            = default;
	constexpr Vector4& operator=(Vector4&&) noexcept = default;

	//* operator [compound assignment] <Vector4> *//

	constexpr Vector4& operator+=(const Vector4& rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
	constexpr Vector4& operator-=(const Vector4& rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
	constexpr Vector4& operator*=(const Vector4& rhs) noexcept { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
	constexpr Vector4& operator/=(const Vector4& rhs) noexcept { x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }

	//* operator [compound assignment] <T> *//

	constexpr Vector4& operator*=(const T rhs) noexcept { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return *this; }
	constexpr Vector4& operator/=(const T rhs) noexcept { x /= rhs; y /= rhs; z /= rhs; w /= rhs; return *this; }

	//* operator [binary] <Vector4> *//

	constexpr Vector4 operator+(const Vector4& rhs) const noexcept { return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w }; }
	constexpr Vector4 operator-(const Vector4& rhs) const noexcept { return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w }; }
	constexpr Vector4 operator*(const Vector4& rhs) const noexcept { return { x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w }; }
	constexpr Vector4 operator/(const Vector4& rhs) const noexcept { return { x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w }; }

	//* operator [binary] <T> *//

	constexpr Vector4 operator*(const T rhs) const noexcept { return { x * rhs, y * rhs, z * rhs, w * rhs }; }
	friend constexpr Vector4 operator*(const T lhs, const Vector4& rhs) noexcept { return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w }; }

	constexpr Vector4 operator/(const T rhs) const noexcept { return { x / rhs, y / rhs, z / rhs, w / rhs }; }
	friend constexpr Vector4 operator/(const T lhs, const Vector4& rhs) noexcept { return { lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w }; }

	//* operator [cast] <Vector4> *//

	template <typename U>
	explicit constexpr operator Vector4<U>() const noexcept { return { static_cast<U>(x), static_cast<U>(y), static_cast<U>(z), static_cast<U>(w) }; }

	//* operator [unary] *//

	constexpr Vector4 operator+() const noexcept { return *this; }
	constexpr Vector4 operator-() const noexcept { return { -x, -y, -z, -w }; }

	//* operator [access] *//

	constexpr T& operator[](size_t index) noexcept { return data[index]; }
	constexpr const T& operator[](size_t index) const noexcept { return data[index]; }

	constexpr T* operator&() noexcept { return data.data(); }
	constexpr const T* operator&() const noexcept { return data.data(); }

	//* constant value methods *//

	constexpr static Vector4 Origin() noexcept { return { T(0), T(0), T(0), T(0) }; }

	constexpr static Vector4 Unit() noexcept { return { T(1), T(1), T(1), T(1) }; }

	constexpr static Vector4 Infinity() noexcept { return { std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity() }; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	union {
#pragma warning(push)
#pragma warning(disable:4201) // [C4201](https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4201) 
		struct {
			T x, y, z, w;
		};

		std::array<T, 4> data;
#pragma warning(pop)
	};

};

////////////////////////////////////////////////////////////////////////////////////////////
// std::formatter - Vector4<T>
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct std::formatter<Vector4<T>, char> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Vector4<T>& v, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {}, {})", v.x, v.y, v.z, v.w);
	}
	
};

template <typename T>
struct std::formatter<Vector4<T>, wchar_t> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::wformat_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Vector4<T>& v, std::wformat_context& ctx) const {
		return std::format_to(ctx.out(), L"({}, {}, {}, {})", v.x, v.y, v.z, v.w);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Vector4 utility
////////////////////////////////////////////////////////////////////////////////////////////

//* integral types *//

using Vector4i  = Vector4<std::int32_t>;
using Vector4ui = Vector4<std::uint32_t>;
using Vector4sz = Vector4<std::size_t>;

//* floating-point types *//

using Vector4f = Vector4<float>;
using Vector4d = Vector4<double>;

////////////////////////////////////////////////////////////////////////////////////////////
// Mathmatical namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Math {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Algorithm methods
	////////////////////////////////////////////////////////////////////////////////////////////

	template <typename T>
	Vector4<T> Clamp(const Vector4<T>& v, const Vector4<T>& min, const Vector4<T>& max) noexcept {
		return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y), std::clamp(v.z, min.z, max.z) };
	}	

	template <typename T>
	Vector4<T> Clamp(const Vector4<T>& v, T min, T max) noexcept {
		return { std::clamp(v.x, min, max), std::clamp(v.y, min, max), std::clamp(v.z, min, max) };
	}

	template <typename T>
	Vector4<T> Min(const Vector4<T>& v, const Vector4<T>& min) noexcept {
		return { std::min(v.x, min.x), std::min(v.y, min.y), std::min(v.z, min.z) };
	}

	template <typename T>
	Vector4<T> Min(const Vector4<T>& v, T min) noexcept {
		return { std::min(v.x, min), std::min(v.y, min), std::min(v.z, min) };
	}

	template <typename T>
	Vector4<T> Max(const Vector4<T>& v, const Vector4<T>& max) noexcept {
		return { std::max(v.x, max.x), std::max(v.y, max.y), std::max(v.z, max.z) };
	}

	template <typename T>
	Vector4<T> Max(const Vector4<T>& v, T max) noexcept {
		return { std::max(v.x, max), std::max(v.y, max), std::max(v.z, max) };
	}

	template <typename T>
	Vector4<T> Saturate(const Vector4<T>& v) noexcept {
		return { std::clamp(v.x, T(0.0), T(1.0)), std::clamp(v.y, T(0.0), T(1.0)), std::clamp(v.z, T(0.0), T(1.0)) };
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mathmatical methods
	////////////////////////////////////////////////////////////////////////////////////////////

	template <typename T> requires std::floating_point<T>
	Vector4<T> Abs(const Vector4<T>& v) noexcept {
		return { std::abs(v.x), std::abs(v.y), std::abs(v.z) };
	}

	template <typename T> requires std::floating_point<T>
	Vector4<T> Floor(const Vector4<T>& v) noexcept {
		return { std::floor(v.x), std::floor(v.y), std::floor(v.z) };
	}

}
