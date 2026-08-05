#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>
#include <array>
#include <concepts>
#include <format>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------

template <std::integral T>
struct Color4Integral; //!< integral color structure template

template <std::floating_point T>
struct Color4floating; //!< floating-point color structure template

////////////////////////////////////////////////////////////////////////////////////////////
// Color4Integral structure
////////////////////////////////////////////////////////////////////////////////////////////
template <std::integral T>
struct Color4Integral {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Color4Integral() noexcept = default;
	constexpr Color4Integral(T _r, T _g, T _b, T _a = T(255)) noexcept : r(_r), g(_g), b(_b), a(_a) {};

	//* operator [copy / move] <Color4Integral> *//

	constexpr Color4Integral(const Color4Integral&) noexcept            = default;
	constexpr Color4Integral& operator=(const Color4Integral&) noexcept = default;

	constexpr Color4Integral(Color4Integral&&) noexcept            = default;
	constexpr Color4Integral& operator=(Color4Integral&&) noexcept = default;

	//* operator [compound assignment] <Color4Integral> *//

	constexpr Color4Integral& operator+=(const Color4Integral& rhs) noexcept { r += rhs.r; g += rhs.g; b += rhs.b; a += rhs.a; return *this; }
	constexpr Color4Integral& operator-=(const Color4Integral& rhs) noexcept { r -= rhs.r; g -= rhs.g; b -= rhs.b; a -= rhs.a; return *this; }
	constexpr Color4Integral& operator*=(const Color4Integral& rhs) noexcept { r *= rhs.r; g *= rhs.g; b *= rhs.b; a *= rhs.a; return *this; }
	constexpr Color4Integral& operator/=(const Color4Integral& rhs) noexcept { r /= rhs.r; g /= rhs.g; b /= rhs.b; a /= rhs.a; return *this; }

	//* operator [compound assignment] <T> *//

	constexpr Color4Integral& operator*=(const T rhs) noexcept { r *= rhs; g *= rhs; b *= rhs; a *= rhs; return *this; }
	constexpr Color4Integral& operator/=(const T rhs) noexcept { r /= rhs; g /= rhs; b /= rhs; a /= rhs; return *this; }

	//* operator [binary] <Color4Integral> *//

	constexpr Color4Integral operator+(const Color4Integral& rhs) const noexcept { return { r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a }; }
	constexpr Color4Integral operator-(const Color4Integral& rhs) const noexcept { return { r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a }; }
	constexpr Color4Integral operator*(const Color4Integral& rhs) const noexcept { return { r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a }; }
	constexpr Color4Integral operator/(const Color4Integral& rhs) const noexcept { return { r / rhs.r, g / rhs.g, b / rhs.b, a / rhs.a }; }

	//* operator [binary] <T> *//

	constexpr Color4Integral operator*(const T rhs) const noexcept { return { r * rhs, g * rhs, b * rhs, a * rhs }; }
	friend constexpr Color4Integral operator*(const T lhs, const Color4Integral& rhs) noexcept { return { lhs * rhs.r, lhs * rhs.g, lhs * rhs.b, lhs * rhs.a }; }

	constexpr Color4Integral operator/(const T rhs) const noexcept { return { r / rhs, g / rhs, b / rhs, a / rhs }; }
	friend constexpr Color4Integral operator/(const T lhs, const Color4Integral& rhs) noexcept { return { lhs / rhs.r, lhs / rhs.g, lhs / rhs.b, lhs / rhs.a }; }

	//* operator [cast] <Color4Integral> *//

	template <typename U>
	constexpr operator Color4Integral<U>() const noexcept {
		return { static_cast<U>(r), static_cast<U>(g), static_cast<U>(b), static_cast<U>(a) };
	}

	//* operator [cast] <Color4floating> *//

	template <std::floating_point U>
	explicit constexpr operator Color4floating<U>() const noexcept {
		return { static_cast<U>(r) / (U)255.0, static_cast<U>(g) / (U)255.0, static_cast<U>(b) / (U)255.0, static_cast<U>(a) / (U)255.0 };
	}

	//* operator [access] *//

	constexpr T& operator[](size_t index) noexcept { return data[index]; }
	constexpr const T& operator[](size_t index) const noexcept { return data[index]; }

	//* convert methods *//

	static constexpr Color4Integral Convert(std::uint32_t code) {
		return {
			static_cast<T>((code >> 3 * 8) & 0xFF),
			static_cast<T>((code >> 2 * 8) & 0xFF),
			static_cast<T>((code >> 1 * 8) & 0xFF),
			static_cast<T>((code >> 0 * 8) & 0xFF)
		};
	}

	//* constant value methods *//

	constexpr static Color4Integral Black() noexcept { return { T(0), T(0), T(0), T(255) }; }

	constexpr static Color4Integral White() noexcept { return { T(255), T(255), T(255), T(255) }; }

	constexpr static Color4Integral Red() noexcept { return { T(255), T(0), T(0), T(255) }; }

	constexpr static Color4Integral Green() noexcept { return { T(0), T(255), T(0), T(255) }; }

	constexpr static Color4Integral Blue() noexcept { return { T(0), T(0), T(255), T(255) }; }

	constexpr static Color4Integral Yellow() noexcept { return { T(255), T(255), T(0), T(255) }; }

	constexpr static Color4Integral Cyan() noexcept { return { T(0), T(255), T(255), T(255) }; }

	constexpr static Color4Integral Magenta() noexcept { return { T(255), T(0), T(255), T(255) }; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	union {
#pragma warning(push)
#pragma warning(disable:4201) // [C4201](https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4201) 
		struct {
			T x, y, z, w;
		};

		struct {
			T r, g, b, a;
		};

		std::array<T, 4> data;
#pragma warning(pop)
	};

};

////////////////////////////////////////////////////////////////////////////////////////////
// std::format - Color4Integral<T>
////////////////////////////////////////////////////////////////////////////////////////////

template <std::integral T>
struct std::formatter<Color4Integral<T>, char> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Color4Integral<T>& color, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {}, {})", color.r, color.g, color.b, color.a);
	}

};

template <std::integral T>
struct std::formatter<Color4Integral<T>, wchar_t> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::wformat_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Color4Integral<T>& color, std::wformat_context& ctx) const {
		return std::format_to(ctx.out(), L"({}, {}, {}, {})", color.r, color.g, color.b, color.a);
	}

};



////////////////////////////////////////////////////////////////////////////////////////////
// Color4floating structure
////////////////////////////////////////////////////////////////////////////////////////////
template <std::floating_point T>
struct Color4floating {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Color4floating() noexcept = default;
	constexpr Color4floating(T _r, T _g, T _b, T _a = T(1)) noexcept : r(_r), g(_g), b(_b), a(_a) {};

	//* operator [copy / move] <Color4floating> *//

	constexpr Color4floating(const Color4floating&) noexcept            = default;
	constexpr Color4floating& operator=(const Color4floating&) noexcept = default;

	constexpr Color4floating(Color4floating&&) noexcept            = default;
	constexpr Color4floating& operator=(Color4floating&&) noexcept = default;

	//* operator [compound assignment] <Color4floating> *//

	constexpr Color4floating& operator+=(const Color4floating& rhs) noexcept { r += rhs.r; g += rhs.g; b += rhs.b; a += rhs.a; return *this; }
	constexpr Color4floating& operator-=(const Color4floating& rhs) noexcept { r -= rhs.r; g -= rhs.g; b -= rhs.b; a -= rhs.a; return *this; }
	constexpr Color4floating& operator*=(const Color4floating& rhs) noexcept { r *= rhs.r; g *= rhs.g; b *= rhs.b; a *= rhs.a; return *this; }
	constexpr Color4floating& operator/=(const Color4floating& rhs) noexcept { r /= rhs.r; g /= rhs.g; b /= rhs.b; a /= rhs.a; return *this; }

	//* operator [compound assignment] <T> *//
	
	constexpr Color4floating& operator*=(const T& rhs) noexcept { r *= rhs; g *= rhs; b *= rhs; a *= rhs; return *this; }
	constexpr Color4floating& operator/=(const T& rhs) noexcept { r /= rhs; g /= rhs; b /= rhs; a /= rhs; return *this; }

	//* operator [binary] <Color4floating> *//

	constexpr Color4floating operator+(const Color4floating& rhs) const noexcept { return { r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a }; }
	constexpr Color4floating operator-(const Color4floating& rhs) const noexcept { return { r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a }; }
	constexpr Color4floating operator*(const Color4floating& rhs) const noexcept { return { r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a }; }
	constexpr Color4floating operator/(const Color4floating& rhs) const noexcept { return { r / rhs.r, g / rhs.g, b / rhs.b, a / rhs.a }; }

	//* operator [binary] <T> *//

	constexpr Color4floating operator*(const T& rhs) const noexcept { return { r * rhs, g * rhs, b * rhs, a * rhs }; }
	friend constexpr Color4floating operator*(const T& lhs, const Color4floating& rhs) noexcept { return { lhs * rhs.r, lhs * rhs.g, lhs * rhs.b, lhs * rhs.a }; }

	constexpr Color4floating operator/(const T& rhs) const noexcept { return { r / rhs, g / rhs, b / rhs, a / rhs }; }
	friend constexpr Color4floating operator/(const T& lhs, const Color4floating& rhs) noexcept { return { lhs / rhs.r, lhs / rhs.g, lhs / rhs.b, lhs / rhs.a }; }

	//* operator [cast] <Color4floating> *//

	template <typename U>
	constexpr operator Color4floating<U>() const noexcept {
		return { static_cast<U>(r), static_cast<U>(g), static_cast<U>(b), static_cast<U>(a) };
	}

	//* operator [cast] <Color4floating> *//

	template <std::integral U>
	explicit constexpr operator Color4Integral<U>() const noexcept {
		return { static_cast<U>(r * (U)255.0), static_cast<U>(g * (U)255.0), static_cast<U>(b * (U)255.0), static_cast<U>(a * (U)255.0) };
	}

	//* operator [access] *//

	constexpr T& operator[](size_t index) noexcept { return data[index]; }
	constexpr const T& operator[](size_t index) const noexcept { return data[index]; }

	constexpr T* operator&() noexcept { return data.data(); }
	constexpr const T* operator&() const noexcept { return data.data(); }

	//* convert methods *//

	static constexpr Color4floating Convert(std::uint32_t code) {
		return {
			static_cast<T>((code >> 3 * 8) & 0xFF) / (T)255.0,
			static_cast<T>((code >> 2 * 8) & 0xFF) / (T)255.0,
			static_cast<T>((code >> 1 * 8) & 0xFF) / (T)255.0,
			static_cast<T>((code >> 0 * 8) & 0xFF) / (T)255.0
		};
	}

	//* color container methods *//

	constexpr T* Ptr() noexcept { return data.data(); }
	constexpr const T* Ptr() const noexcept { return data.data(); }

	//* constant value methods *//

	constexpr static Color4floating Black() noexcept { return { T(0.0), T(0.0), T(0.0), T(1.0) }; }

	constexpr static Color4floating White() noexcept { return { T(1.0), T(1.0), T(1.0), T(1.0) }; }

	constexpr static Color4floating Red() noexcept { return { T(1.0), T(0.0), T(0.0), T(1.0) }; }

	constexpr static Color4floating Green() noexcept { return { T(0.0), T(1.0), T(0.0), T(1.0) }; }

	constexpr static Color4floating Blue() noexcept { return { T(0.0), T(0.0), T(1.0), T(1.0) }; }

	constexpr static Color4floating Yellow() noexcept { return { T(1.0), T(1.0), T(0.0), T(1.0) }; }

	constexpr static Color4floating Cyan() noexcept { return { T(0.0), T(1.0), T(1.0), T(1.0) }; }

	constexpr static Color4floating Magenta() noexcept { return { T(1.0), T(0.0), T(1.0), T(1.0) }; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	union {
#pragma warning(push)
#pragma warning(disable:4201) // [C4201](https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4201)
		struct {
			T x, y, z, w;
		};

		struct {
			T r, g, b, a;
		};

		std::array<T, 4> data;
#pragma warning(pop)
	};

};

////////////////////////////////////////////////////////////////////////////////////////////
// std::format - Color4floating<T>
////////////////////////////////////////////////////////////////////////////////////////////

template <std::floating_point T>
struct std::formatter<Color4floating<T>, char> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Color4floating<T>& color, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {}, {})", color.r, color.g, color.b, color.a);
	}

};

template <std::floating_point T>
struct std::formatter<Color4floating<T>, wchar_t> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::wformat_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Color4floating<T>& color, std::wformat_context& ctx) const {
		return std::format_to(ctx.out(), L"({}, {}, {}, {})", color.r, color.g, color.b, color.a);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Color4 Utilities
////////////////////////////////////////////////////////////////////////////////////////////

//* Color4 integral *//

using Color4i  = Color4Integral<std::int32_t>;
using Color4ui = Color4Integral<std::uint32_t>;

//* Color4 floating-point *//

using Color4f = Color4floating<float>;
using Color4d = Color4floating<double>;
