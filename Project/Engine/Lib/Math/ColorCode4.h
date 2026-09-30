#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>
#include <array>
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// ColorCode4 structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ColorCode4 {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr ColorCode4() noexcept = default;
	constexpr ColorCode4(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) noexcept : r(_r), g(_g), b(_b), a(_a) {};
	constexpr ColorCode4(uint32_t code) noexcept : r((code >> 24) & 0xFF), g((code >> 16) & 0xFF), b((code >> 8) & 0xFF), a((code >> 0) & 0xFF) {}; //!< 0xRRGGBBAA

	//* color code methods *//

	constexpr uint32_t Code() const noexcept { return (static_cast<uint32_t>(r) << 24) | (static_cast<uint32_t>(g) << 16) | (static_cast<uint32_t>(b) << 8) | (static_cast<uint32_t>(a) << 0); }

	//* operator [copy / move] <ColorCode4> *//

	constexpr ColorCode4(const ColorCode4&) noexcept            = default;
	constexpr ColorCode4& operator=(const ColorCode4&) noexcept = default;

	constexpr ColorCode4(ColorCode4&&) noexcept            = default;
	constexpr ColorCode4& operator=(ColorCode4&&) noexcept = default;

	//* operator [access] *//

	constexpr uint8_t& operator[](size_t index) { return data[index]; }
	constexpr const uint8_t& operator[](size_t index) const { return data[index]; }

	//* color container methods *//

	constexpr uint8_t* Data() noexcept { return data.data(); }
	constexpr const uint8_t* Data() const noexcept { return data.data(); }

	//* constant value methods *//

	constexpr static ColorCode4 Black() noexcept { return { 0, 0, 0, 0xFF }; }

	constexpr static ColorCode4 White() noexcept { return { 0xFF, 0xFF, 0xFF, 0xFF }; }

	constexpr static ColorCode4 Red() noexcept { return { 0xFF, 0, 0, 0xFF }; }

	constexpr static ColorCode4 Green() noexcept { return { 0, 0xFF, 0, 0xFF }; }

	constexpr static ColorCode4 Blue() noexcept { return { 0, 0, 0xFF, 0xFF }; }

	constexpr static ColorCode4 Yellow() noexcept { return { 0xFF, 0xFF, 0, 0xFF }; }

	constexpr static ColorCode4 Cyan() noexcept { return { 0, 0xFF, 0xFF, 0xFF }; }

	constexpr static ColorCode4 Magenta() noexcept { return { 0xFF, 0, 0xFF, 0xFF }; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	union {
#pragma warning(push)
#pragma warning(disable:4201) // [C4201](https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4201) 
		struct {
			uint8_t x, y, z, w;
		};

		struct {
			uint8_t r, g, b, a;
		};

		std::array<uint8_t, 4> data;
#pragma warning(pop)
	};

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// std::formatter - ColorCode4
////////////////////////////////////////////////////////////////////////////////////////////

template <>
struct std::formatter<ColorCode4, char> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const ColorCode4& v, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {}, {})", v.r, v.g, v.b, v.a);
	}

};
