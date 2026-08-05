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
#include <algorithm>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// Vector2 structure
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct Vector2 {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Vector2() noexcept = default;
	constexpr Vector2(T _x, T _y) noexcept : x(_x), y(_y) {};

	//* operator [copy / move] <Vector2> *//

	constexpr Vector2(const Vector2&) noexcept            = default;
	constexpr Vector2& operator=(const Vector2&) noexcept = default;

	constexpr Vector2(Vector2&&) noexcept            = default;
	constexpr Vector2& operator=(Vector2&&) noexcept = default;

	//* operator [compound assignment] <Vector2> *//

	constexpr Vector2& operator+=(const Vector2& rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }
	constexpr Vector2& operator-=(const Vector2& rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }
	constexpr Vector2& operator*=(const Vector2& rhs) noexcept { x *= rhs.x; y *= rhs.y; return *this; }
	constexpr Vector2& operator/=(const Vector2& rhs) noexcept { x /= rhs.x; y /= rhs.y; return *this; }

	//* operator [compound assignment] <T> *//

	constexpr Vector2& operator*=(const T rhs) noexcept { x *= rhs; y *= rhs; return *this; }
	constexpr Vector2& operator/=(const T rhs) noexcept { x /= rhs; y /= rhs; return *this; }

	//* operator [binary] <Vector2> *//

	constexpr Vector2 operator+(const Vector2& rhs) const noexcept { return { x + rhs.x, y + rhs.y }; }
	constexpr Vector2 operator-(const Vector2& rhs) const noexcept { return { x - rhs.x, y - rhs.y }; }
	constexpr Vector2 operator*(const Vector2& rhs) const noexcept { return { x * rhs.x, y * rhs.y }; }
	constexpr Vector2 operator/(const Vector2& rhs) const noexcept { return { x / rhs.x, y / rhs.y }; }


	//* operator [binary] <T> *//

	constexpr Vector2 operator*(const T rhs) const noexcept { return { x * rhs, y * rhs }; }
	friend constexpr Vector2 operator*(const T lhs, const Vector2& rhs) noexcept { return { lhs * rhs.x, lhs * rhs.y }; }

	constexpr Vector2 operator/(const T rhs) const noexcept { return { x / rhs, y / rhs }; }
	friend constexpr Vector2 operator/(const T lhs, const Vector2& rhs) noexcept { return { lhs / rhs.x, lhs / rhs.y }; }
	

	//* operator [cast] <Vector2> *//

	template <typename U>
	explicit constexpr operator Vector2<U>() const noexcept { return { static_cast<U>(x), static_cast<U>(y) }; }

	//* operator [unary] *//

	constexpr Vector2 operator+() const noexcept { return *this; }
	constexpr Vector2 operator-() const noexcept { return { -x, -y }; }

	//* operator [access] *//

	constexpr T& operator[](size_t index) noexcept { return data[index]; }
	constexpr const T& operator[](size_t index) const noexcept { return data[index]; }

	constexpr T* operator&() noexcept { return data.data(); }
	constexpr const T* operator&() const noexcept { return data.data(); }

	//* constant value methods *//

	constexpr static Vector2 Origin() noexcept { return { T(0), T(0) }; }

	constexpr static Vector2 Unit() noexcept { return { T(1), T(1) }; }

	constexpr static Vector2 Infinity() noexcept { return { std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity() }; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	union {
#pragma warning(push)
#pragma warning(disable:4201) // [C4201](https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4201) 
		struct {
			T x, y;
		};

		std::array<T, 2> data;
#pragma warning(pop)
	};

};

////////////////////////////////////////////////////////////////////////////////////////////
// std::formatter - Vector2<T>
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct std::formatter<Vector2<T>, char> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Vector2<T>& v, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "({}, {})", v.x, v.y);
	}
	
};

template <typename T>
struct std::formatter<Vector2<T>, wchar_t> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::wformat_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Vector2<T>& v, std::wformat_context& ctx) const {
		return std::format_to(ctx.out(), L"({}, {})", v.x, v.y);
	}
	
};


////////////////////////////////////////////////////////////////////////////////////////////
// Vector2 Utilities
////////////////////////////////////////////////////////////////////////////////////////////

//* integral types *//

using Vector2i  = Vector2<std::int32_t>;
using Vector2ui = Vector2<std::uint32_t>;
using Vector2sz = Vector2<std::size_t>;

//* floating-point types *//

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;

////////////////////////////////////////////////////////////////////////////////////////////
// Mathmatical namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Math {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Algorithm methods
	////////////////////////////////////////////////////////////////////////////////////////////

	template <typename T>
	Vector2<T> Clamp(const Vector2<T>& v, const Vector2<T>& min, const Vector2<T>& max) noexcept {
		return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y) };
	}

	template <typename T>
	Vector2<T> Clamp(const Vector2<T>& v, T min, T max) noexcept {
		return { std::clamp(v.x, min, max), std::clamp(v.y, min, max) };
	}

	template <typename T>
	Vector2<T> Min(const Vector2<T>& v, const Vector2<T>& min) noexcept {
		return { std::min(v.x, min.x), std::min(v.y, min.y) };
	}

	template <typename T>
	Vector2<T> Min(const Vector2<T>& v, T min) noexcept {
		return { std::min(v.x, min), std::min(v.y, min) };
	}

	template <typename T>
	Vector2<T> Max(const Vector2<T>& v, const Vector2<T>& max) noexcept {
		return { std::max(v.x, max.x), std::max(v.y, max.y) };
	}

	template <typename T>
	Vector2<T> Max(const Vector2<T>& v, T max) noexcept {
		return { std::max(v.x, max), std::max(v.y, max) };
	}

	template <typename T>
	Vector2<T> Saturate(const Vector2<T>& v) noexcept {
		return { std::clamp(v.x, T(0.0), T(1.0)), std::clamp(v.y, T(0.0), T(1.0)) };
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mathmatical methods
	////////////////////////////////////////////////////////////////////////////////////////////

	template <typename T> requires std::floating_point<T>
	Vector2<T> Abs(const Vector2<T>& v) noexcept {
		return { std::abs(v.x), std::abs(v.y) };
	}

	template <typename T> requires std::floating_point<T>
	Vector2<T> Floor(const Vector2<T>& v) noexcept {
		return { std::floor(v.x), std::floor(v.y) };
	}

	template <typename T> requires std::floating_point<T>
	T Length(const Vector2<T>& v) noexcept {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	template <typename T> requires std::floating_point<T>
	T Distance(const Vector2<T>& lhs, const Vector2<T>& rhs) noexcept {
		return Length(lhs - rhs);
	}

	template <typename T> requires std::floating_point<T>
	Vector2<T> Normalize(const Vector2<T>& v) noexcept {
		T len = Length(v);
		return len != T(0.0) ? v / len : Vector2<T>();
	}

	template <typename T> requires std::floating_point<T>
	T Dot(const Vector2<T>& lhs, const Vector2<T>& rhs) noexcept {
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	template <typename T> requires std::floating_point<T>
	T Cross(const Vector2<T>& lhs, const Vector2<T>& rhs) noexcept {
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}

	template <typename T> requires std::floating_point<T>
	Vector2<T> Lerp(const Vector2<T>& x, const Vector2<T>& y, T t) noexcept {
		return x * (T(1.0) - t) + y * t;
	}

	template <typename T> requires std::floating_point<T>
	Vector2<T> Reflect(const Vector2<T>& v, const Vector2<T>& n) noexcept {
		return v - T(2.0) * Dot(v, n) * n;
	}

}
