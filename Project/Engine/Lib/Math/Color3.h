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
struct Color3Integral; //!< integral color structure template

template <std::floating_point T>
struct Color3floating; //!< floating-point color structure template

////////////////////////////////////////////////////////////////////////////////////////////
// Color3Integral structure
////////////////////////////////////////////////////////////////////////////////////////////
template <std::integral T>
struct Color3Integral {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Color3Integral() noexcept = default;
	constexpr Color3Integral(T _r, T _g, T _b) noexcept : r(_r), g(_g), b(_b) {};

	//* operator [copy / move] <Color3Integral> *//

	constexpr Color3Integral(const Color3Integral&) noexcept            = default;
	constexpr Color3Integral& operator=(const Color3Integral&) noexcept = default;

	constexpr Color3Integral(Color3Integral&&) noexcept            = default;
	constexpr Color3Integral& operator=(Color3Integral&&) noexcept = default;

	//* operator [compound assignment] <Color3Integral> *//

	constexpr Color3Integral& operator+=(const Color3Integral& rhs) noexcept { r += rhs.r; g += rhs.g; b += rhs.b; return *this; }
	constexpr Color3Integral& operator-=(const Color3Integral& rhs) noexcept { r -= rhs.r; g -= rhs.g; b -= rhs.b; return *this; }
	constexpr Color3Integral& operator*=(const Color3Integral& rhs) noexcept { r *= rhs.r; g *= rhs.g; b *= rhs.b; return *this; }
	constexpr Color3Integral& operator/=(const Color3Integral& rhs) noexcept { r /= rhs.r; g /= rhs.g; b /= rhs.b; return *this; }

	//* operator [compound assignment] <T> *//

	constexpr Color3Integral& operator*=(const T rhs) noexcept { r *= rhs; g *= rhs; b *= rhs; return *this; }
	constexpr Color3Integral& operator/=(const T rhs) noexcept { r /= rhs; g /= rhs; b /= rhs; return *this; }

	//* operator [binary] <Color3Integral> *//

	constexpr Color3Integral operator+(const Color3Integral& rhs) const noexcept { return { r + rhs.r, g + rhs.g, b + rhs.b }; }
	constexpr Color3Integral operator-(const Color3Integral& rhs) const noexcept { return { r - rhs.r, g - rhs.g, b - rhs.b }; }
	constexpr Color3Integral operator*(const Color3Integral& rhs) const noexcept { return { r * rhs.r, g * rhs.g, b * rhs.b }; }
	constexpr Color3Integral operator/(const Color3Integral& rhs) const noexcept { return { r / rhs.r, g / rhs.g, b / rhs.b }; }

	//* operator [binary] <T> *//

	constexpr Color3Integral operator*(const T rhs) const noexcept { return { r * rhs, g * rhs, b * rhs }; }
	constexpr Color3Integral operator/(const T rhs) const noexcept { return { r / rhs, g / rhs, b / rhs }; }

	//* operator [cast] <Color3Integral> *//

	template <typename U>
	constexpr operator Color3Integral<U>() const noexcept {
		return { static_cast<U>(r), static_cast<U>(g), static_cast<U>(b) };
	}

	//* operator [cast] <Color3floating> *//

	template <std::floating_point U>
	explicit constexpr operator Color3floating<U>() const noexcept {
		return { static_cast<U>(r) / (U)255.0, static_cast<U>(g) / (U)255.0, static_cast<U>(b) / (U)255.0 };
	}

	//* operator [access] *//

	constexpr T& operator[](size_t index) noexcept { return data[index]; }
	constexpr const T& operator[](size_t index) const noexcept { return data[index]; }

	constexpr T* operator&() noexcept { return data.data(); }
	constexpr const T* operator&() const noexcept { return data.data(); }

	//* convert methods *//

	static constexpr Color3Integral Convert(std::uint32_t code) {
		return {
			static_cast<T>((code >> 2 * 8) & 0xFF),
			static_cast<T>((code >> 1 * 8) & 0xFF),
			static_cast<T>((code >> 0 * 8) & 0xFF)
		};
	}

	//* constant value methods *//

	constexpr static Color3Integral Black() noexcept { return { T(0), T(0), T(0) }; }

	constexpr static Color3Integral White() noexcept { return { T(255), T(255), T(255) }; }

	constexpr static Color3Integral Red() noexcept { return { T(255), T(0), T(0) }; }

	constexpr static Color3Integral Green() noexcept { return { T(0), T(255), T(0) }; }

	constexpr static Color3Integral Blue() noexcept { return { T(0), T(0), T(255) }; }

	constexpr static Color3Integral Yellow() noexcept { return { T(255), T(255), T(0) }; }

	constexpr static Color3Integral Cyan() noexcept { return { T(0), T(255), T(255) }; }

	constexpr static Color3Integral Magenta() noexcept { return { T(255), T(0), T(255) }; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	union {
#pragma warning(push)
#pragma warning(disable: 4201) // [C4201](https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4201)
		struct {
			T x, y, z;
		};

		struct {
			T r, g, b;
		};

		std::array<T, 3> data;
#pragma warning(pop)
	};

};

////////////////////////////////////////////////////////////////////////////////////////////
// std::formatter - Color3Integral<T>
////////////////////////////////////////////////////////////////////////////////////////////

template <std::integral T>
struct std::formatter<Color3Integral<T>, char> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Color3Integral<T>& v, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {})", v.r, v.g, v.b);
	}

};

template <std::integral T>
struct std::formatter<Color3Integral<T>, wchar_t> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::wformat_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Color3Integral<T>& v, std::wformat_context& ctx) const {
		return std::format_to(ctx.out(), L"({}, {}, {})", v.r, v.g, v.b);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Color3floating structure
////////////////////////////////////////////////////////////////////////////////////////////
template <std::floating_point T>
struct Color3floating {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Color3floating() noexcept = default;
	constexpr Color3floating(T _r, T _g, T _b) noexcept : r(_r), g(_g), b(_b) {};

	//* operator [copy / move] <Color3floating> *//

	constexpr Color3floating(const Color3floating&) noexcept            = default;
	constexpr Color3floating& operator=(const Color3floating&) noexcept = default;

	constexpr Color3floating(Color3floating&&) noexcept            = default;
	constexpr Color3floating& operator=(Color3floating&&) noexcept = default;

	//* operator [compound assignment] <Color3floating> *//

	constexpr Color3floating& operator+=(const Color3floating& rhs) noexcept { r += rhs.r; g += rhs.g; b += rhs.b; return *this; }
	constexpr Color3floating& operator-=(const Color3floating& rhs) noexcept { r -= rhs.r; g -= rhs.g; b -= rhs.b; return *this; }
	constexpr Color3floating& operator*=(const Color3floating& rhs) noexcept { r *= rhs.r; g *= rhs.g; b *= rhs.b; return *this; }
	constexpr Color3floating& operator/=(const Color3floating& rhs) noexcept { r /= rhs.r; g /= rhs.g; b /= rhs.b; return *this; }

	//* operator [compound assignment] <T> *//
	
	constexpr Color3floating& operator*=(const T& rhs) noexcept { r *= rhs; g *= rhs; b *= rhs; return *this; }
	constexpr Color3floating& operator/=(const T& rhs) noexcept { r /= rhs; g /= rhs; b /= rhs; return *this; }

	//* operator [binary] <Color3floating> *//

	constexpr Color3floating operator+(const Color3floating& rhs) const noexcept { return { r + rhs.r, g + rhs.g, b + rhs.b }; }
	constexpr Color3floating operator-(const Color3floating& rhs) const noexcept { return { r - rhs.r, g - rhs.g, b - rhs.b }; }
	constexpr Color3floating operator*(const Color3floating& rhs) const noexcept { return { r * rhs.r, g * rhs.g, b * rhs.b }; }
	constexpr Color3floating operator/(const Color3floating& rhs) const noexcept { return { r / rhs.r, g / rhs.g, b / rhs.b }; }

	//* operator [binary] <T> *//

	constexpr Color3floating operator*(const T& rhs) const noexcept { return { r * rhs, g * rhs, b * rhs }; }
	constexpr Color3floating operator/(const T& rhs) const noexcept { return { r / rhs, g / rhs, b / rhs }; }

	//* operator [cast] <Color3floating> *//

	template <typename U>
	constexpr operator Color3floating<U>() const noexcept {
		return { static_cast<U>(r), static_cast<U>(g), static_cast<U>(b) };
	}

	//* operator [cast] <Color3floating> *//

	template <std::integral U>
	explicit constexpr operator Color3Integral<U>() const noexcept {
		return { static_cast<U>(r * (U)255.0), static_cast<U>(g * (U)255.0), static_cast<U>(b * (U)255.0) };
	}

	//* operator [access] *//

	constexpr T& operator[](size_t index) noexcept { return data[index]; }
	constexpr const T& operator[](size_t index) const noexcept { return data[index]; }

	//* convert methods *//

	static constexpr Color3Integral Convert(std::uint32_t code) {
		return {
			static_cast<T>((code >> 2 * 8) & 0xFF) / (T)255.0,
			static_cast<T>((code >> 1 * 8) & 0xFF) / (T)255.0,
			static_cast<T>((code >> 0 * 8) & 0xFF) / (T)255.0
		};
	}

	//* constant value methods *//

	constexpr static Color3floating Black() noexcept { return { T(0), T(0), T(0) }; }

	constexpr static Color3floating White() noexcept { return { T(1), T(1), T(1) }; }

	constexpr static Color3floating Red() noexcept { return { T(1), T(0), T(0) }; }

	constexpr static Color3floating Green() noexcept { return { T(0), T(1), T(0) }; }

	constexpr static Color3floating Blue() noexcept { return { T(0), T(0), T(1) }; }

	constexpr static Color3floating Yellow() noexcept { return { T(1), T(1), T(0) }; }

	constexpr static Color3floating Cyan() noexcept { return { T(0), T(1), T(1) }; }

	constexpr static Color3floating Magenta() noexcept { return { T(1), T(0), T(1) }; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	union {
#pragma warning(push)
#pragma warning(disable: 4201) // [C4201](https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4201)
		struct {
			T x, y, z;
		};

		struct {
			T r, g, b;
		};

		std::array<T, 3> data;
#pragma warning(pop)
	};

};

////////////////////////////////////////////////////////////////////////////////////////////
// std::formatter - Color3floating<T>
////////////////////////////////////////////////////////////////////////////////////////////

template <std::floating_point T>
struct std::formatter<Color3floating<T>, char> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Color3floating<T>& v, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {})", v.r, v.g, v.b);
	}

};

template <std::floating_point T>
struct std::formatter<Color3floating<T>, wchar_t> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::wformat_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const Color3floating<T>& v, std::wformat_context& ctx) const {
		return std::format_to(ctx.out(), L"({}, {}, {})", v.r, v.g, v.b);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Color3 Utilities
////////////////////////////////////////////////////////////////////////////////////////////

//* Color3 integral *//

using Color3i  = Color3Integral<std::int32_t>;
using Color3ui = Color3Integral<std::uint32_t>;

//* Color3 floating-point *//

using Color3f = Color3floating<float>;
using Color3d = Color3floating<double>;
