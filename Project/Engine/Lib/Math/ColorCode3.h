#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>
#include <array>
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// ColorCode3 structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ColorCode3 {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr ColorCode3() noexcept = default;
	constexpr ColorCode3(uint8_t _r, uint8_t _g, uint8_t _b) noexcept : r(_r), g(_g), b(_b) {};
	constexpr ColorCode3(uint32_t code) noexcept : r((code >> 16) & 0xFF), g((code >> 8) & 0xFF), b((code >> 0) & 0xFF) {}; //!< 0xRRGGBB

	//* color code methods *//

	constexpr uint32_t Code() const noexcept { return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | (static_cast<uint32_t>(b) << 0); }

	//* operator [copy / move] <ColorCode3> *//

	constexpr ColorCode3(const ColorCode3&) noexcept            = default;
	constexpr ColorCode3& operator=(const ColorCode3&) noexcept = default;

	constexpr ColorCode3(ColorCode3&&) noexcept            = default;
	constexpr ColorCode3& operator=(ColorCode3&&) noexcept = default;

	//* operator [access] *//

	constexpr uint8_t& operator[](size_t index) { return data[index]; }
	constexpr const uint8_t& operator[](size_t index) const { return data[index]; }

	//* color container methods *//

	constexpr uint8_t* Data() noexcept { return data.data(); }
	constexpr const uint8_t* Data() const noexcept { return data.data(); }

	//* constant value methods *//

	constexpr static ColorCode3 Black() noexcept { return { 0, 0, 0 }; }

	constexpr static ColorCode3 White() noexcept { return { 0xFF, 0xFF, 0xFF }; }

	constexpr static ColorCode3 Red() noexcept { return { 0xFF, 0, 0 }; }

	constexpr static ColorCode3 Green() noexcept { return { 0, 0xFF, 0 }; }

	constexpr static ColorCode3 Blue() noexcept { return { 0, 0, 0xFF }; }

	constexpr static ColorCode3 Yellow() noexcept { return { 0xFF, 0xFF, 0 }; }

	constexpr static ColorCode3 Cyan() noexcept { return { 0, 0xFF, 0xFF }; }

	constexpr static ColorCode3 Magenta() noexcept { return { 0xFF, 0, 0xFF }; }

	//=========================================================================================
	// public variables
	//=========================================================================================

	union {
#pragma warning(push)
#pragma warning(disable:4201) // [C4201](https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4201) 
		struct {
			uint8_t x, y, z;
		};

		struct {
			uint8_t r, g, b;
		};

		std::array<uint8_t, 3> data;
#pragma warning(pop)
	};

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// std::formatter - ColorCode3
////////////////////////////////////////////////////////////////////////////////////////////

template <>
struct std::formatter<ColorCode3, char> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const ColorCode3& v, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {})", v.r, v.g, v.b);
	}

};
